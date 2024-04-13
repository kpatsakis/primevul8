vhost_user_get_vring_base(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
	struct vhost_virtqueue *vq = dev->virtqueue[ctx->msg.payload.state.index];
	uint64_t val;

	if (validate_msg_fds(dev, ctx, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	/* We have to stop the queue (virtio) if it is running. */
	vhost_destroy_device_notify(dev);

	dev->flags &= ~VIRTIO_DEV_READY;
	dev->flags &= ~VIRTIO_DEV_VDPA_CONFIGURED;

	/* Here we are safe to get the indexes */
	if (vq_is_packed(dev)) {
		/*
		 * Bit[0:14]: avail index
		 * Bit[15]: avail wrap counter
		 */
		val = vq->last_avail_idx & 0x7fff;
		val |= vq->avail_wrap_counter << 15;
		ctx->msg.payload.state.num = val;
	} else {
		ctx->msg.payload.state.num = vq->last_avail_idx;
	}

	VHOST_LOG_CONFIG(INFO, "(%s) vring base idx:%d file:%d\n",
			dev->ifname, ctx->msg.payload.state.index,
			ctx->msg.payload.state.num);
	/*
	 * Based on current qemu vhost-user implementation, this message is
	 * sent and only sent in vhost_vring_stop.
	 * TODO: cleanup the vring, it isn't usable since here.
	 */
	if (vq->kickfd >= 0)
		close(vq->kickfd);

	vq->kickfd = VIRTIO_UNINITIALIZED_EVENTFD;

	if (vq->callfd >= 0)
		close(vq->callfd);

	vq->callfd = VIRTIO_UNINITIALIZED_EVENTFD;

	vq->signalled_used_valid = false;

	if (vq_is_packed(dev)) {
		rte_free(vq->shadow_used_packed);
		vq->shadow_used_packed = NULL;
	} else {
		rte_free(vq->shadow_used_split);
		vq->shadow_used_split = NULL;
	}

	rte_free(vq->batch_copy_elems);
	vq->batch_copy_elems = NULL;

	rte_free(vq->log_cache);
	vq->log_cache = NULL;

	ctx->msg.size = sizeof(ctx->msg.payload.state);
	ctx->fd_num = 0;

	vhost_user_iotlb_flush_all(vq);

	vring_invalidate(dev, vq);

	return RTE_VHOST_MSG_RESULT_REPLY;
}