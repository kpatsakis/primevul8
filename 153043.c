vhost_user_set_vring_kick(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
	struct vhost_vring_file file;
	struct vhost_virtqueue *vq;
	int expected_fds;

	expected_fds = (ctx->msg.payload.u64 & VHOST_USER_VRING_NOFD_MASK) ? 0 : 1;
	if (validate_msg_fds(dev, ctx, expected_fds) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	file.index = ctx->msg.payload.u64 & VHOST_USER_VRING_IDX_MASK;
	if (ctx->msg.payload.u64 & VHOST_USER_VRING_NOFD_MASK)
		file.fd = VIRTIO_INVALID_EVENTFD;
	else
		file.fd = ctx->fds[0];
	VHOST_LOG_CONFIG(INFO, "(%s) vring kick idx:%d file:%d\n",
			dev->ifname, file.index, file.fd);

	/* Interpret ring addresses only when ring is started. */
	dev = translate_ring_addresses(dev, file.index);
	if (!dev) {
		if (file.fd != VIRTIO_INVALID_EVENTFD)
			close(file.fd);

		return RTE_VHOST_MSG_RESULT_ERR;
	}

	*pdev = dev;

	vq = dev->virtqueue[file.index];

	/*
	 * When VHOST_USER_F_PROTOCOL_FEATURES is not negotiated,
	 * the ring starts already enabled. Otherwise, it is enabled via
	 * the SET_VRING_ENABLE message.
	 */
	if (!(dev->features & (1ULL << VHOST_USER_F_PROTOCOL_FEATURES))) {
		vq->enabled = true;
	}

	if (vq->ready) {
		vq->ready = false;
		vhost_user_notify_queue_state(dev, file.index, 0);
	}

	if (vq->kickfd >= 0)
		close(vq->kickfd);
	vq->kickfd = file.fd;

	if (vq_is_packed(dev)) {
		if (vhost_check_queue_inflights_packed(dev, vq)) {
			VHOST_LOG_CONFIG(ERR, "(%s) failed to inflights for vq: %d\n",
					dev->ifname, file.index);
			return RTE_VHOST_MSG_RESULT_ERR;
		}
	} else {
		if (vhost_check_queue_inflights_split(dev, vq)) {
			VHOST_LOG_CONFIG(ERR, "(%s) failed to inflights for vq: %d\n",
					dev->ifname, file.index);
			return RTE_VHOST_MSG_RESULT_ERR;
		}
	}

	return RTE_VHOST_MSG_RESULT_OK;
}