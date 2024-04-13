static void handle_tx(struct vhost_net *net)
{
	struct vhost_net_virtqueue *nvq = &net->vqs[VHOST_NET_VQ_TX];
	struct vhost_virtqueue *vq = &nvq->vq;
	unsigned out, in;
	int head;
	struct msghdr msg = {
		.msg_name = NULL,
		.msg_namelen = 0,
		.msg_control = NULL,
		.msg_controllen = 0,
		.msg_flags = MSG_DONTWAIT,
	};
	size_t len, total_len = 0;
	int err;
	size_t hdr_size;
	struct socket *sock;
	struct vhost_net_ubuf_ref *uninitialized_var(ubufs);
	bool zcopy, zcopy_used;

	mutex_lock(&vq->mutex);
	sock = vq->private_data;
	if (!sock)
		goto out;

	if (!vq_iotlb_prefetch(vq))
		goto out;

	vhost_disable_notify(&net->dev, vq);
	vhost_net_disable_vq(net, vq);

	hdr_size = nvq->vhost_hlen;
	zcopy = nvq->ubufs;

	for (;;) {
		/* Release DMAs done buffers first */
		if (zcopy)
			vhost_zerocopy_signal_used(net, vq);


		head = vhost_net_tx_get_vq_desc(net, vq, vq->iov,
						ARRAY_SIZE(vq->iov),
						&out, &in);
		/* On error, stop handling until the next kick. */
		if (unlikely(head < 0))
			break;
		/* Nothing new?  Wait for eventfd to tell us they refilled. */
		if (head == vq->num) {
			if (unlikely(vhost_enable_notify(&net->dev, vq))) {
				vhost_disable_notify(&net->dev, vq);
				continue;
			}
			break;
		}
		if (in) {
			vq_err(vq, "Unexpected descriptor format for TX: "
			       "out %d, int %d\n", out, in);
			break;
		}
		/* Skip header. TODO: support TSO. */
		len = iov_length(vq->iov, out);
		iov_iter_init(&msg.msg_iter, WRITE, vq->iov, out, len);
		iov_iter_advance(&msg.msg_iter, hdr_size);
		/* Sanity check */
		if (!msg_data_left(&msg)) {
			vq_err(vq, "Unexpected header len for TX: "
			       "%zd expected %zd\n",
			       len, hdr_size);
			break;
		}
		len = msg_data_left(&msg);

		zcopy_used = zcopy && len >= VHOST_GOODCOPY_LEN
				   && !vhost_exceeds_maxpend(net)
				   && vhost_net_tx_select_zcopy(net);

		/* use msg_control to pass vhost zerocopy ubuf info to skb */
		if (zcopy_used) {
			struct ubuf_info *ubuf;
			ubuf = nvq->ubuf_info + nvq->upend_idx;

			vq->heads[nvq->upend_idx].id = cpu_to_vhost32(vq, head);
			vq->heads[nvq->upend_idx].len = VHOST_DMA_IN_PROGRESS;
			ubuf->callback = vhost_zerocopy_callback;
			ubuf->ctx = nvq->ubufs;
			ubuf->desc = nvq->upend_idx;
			refcount_set(&ubuf->refcnt, 1);
			msg.msg_control = ubuf;
			msg.msg_controllen = sizeof(ubuf);
			ubufs = nvq->ubufs;
			atomic_inc(&ubufs->refcount);
			nvq->upend_idx = (nvq->upend_idx + 1) % UIO_MAXIOV;
		} else {
			msg.msg_control = NULL;
			ubufs = NULL;
		}

		total_len += len;
		if (total_len < VHOST_NET_WEIGHT &&
		    !vhost_vq_avail_empty(&net->dev, vq) &&
		    likely(!vhost_exceeds_maxpend(net))) {
			msg.msg_flags |= MSG_MORE;
		} else {
			msg.msg_flags &= ~MSG_MORE;
		}

		/* TODO: Check specific error and bomb out unless ENOBUFS? */
		err = sock->ops->sendmsg(sock, &msg, len);
		if (unlikely(err < 0)) {
			if (zcopy_used) {
				vhost_net_ubuf_put(ubufs);
				nvq->upend_idx = ((unsigned)nvq->upend_idx - 1)
					% UIO_MAXIOV;
			}
			vhost_discard_vq_desc(vq, 1);
			vhost_net_enable_vq(net, vq);
			break;
		}
		if (err != len)
			pr_debug("Truncated TX packet: "
				 " len %d != %zd\n", err, len);
		if (!zcopy_used)
			vhost_add_used_and_signal(&net->dev, vq, head, 0);
		else
			vhost_zerocopy_signal_used(net, vq);
		vhost_net_tx_packet(net);
		if (unlikely(total_len >= VHOST_NET_WEIGHT)) {
			vhost_poll_queue(&vq->poll);
			break;
		}
	}
out:
	mutex_unlock(&vq->mutex);
}