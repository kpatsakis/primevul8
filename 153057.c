vhost_user_set_vring_call(struct virtio_net **pdev,
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
	VHOST_LOG_CONFIG(INFO, "(%s) vring call idx:%d file:%d\n",
			dev->ifname, file.index, file.fd);

	vq = dev->virtqueue[file.index];

	if (vq->ready) {
		vq->ready = false;
		vhost_user_notify_queue_state(dev, file.index, 0);
	}

	if (vq->callfd >= 0)
		close(vq->callfd);

	vq->callfd = file.fd;

	return RTE_VHOST_MSG_RESULT_OK;
}