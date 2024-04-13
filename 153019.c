vhost_user_set_req_fd(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
	int fd = ctx->fds[0];

	if (validate_msg_fds(dev, ctx, 1) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	if (fd < 0) {
		VHOST_LOG_CONFIG(ERR, "(%s) invalid file descriptor for slave channel (%d)\n",
				dev->ifname, fd);
		return RTE_VHOST_MSG_RESULT_ERR;
	}

	if (dev->slave_req_fd >= 0)
		close(dev->slave_req_fd);

	dev->slave_req_fd = fd;

	return RTE_VHOST_MSG_RESULT_OK;
}