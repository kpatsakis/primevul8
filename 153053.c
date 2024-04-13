vhost_user_set_postcopy_advise(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
#ifdef RTE_LIBRTE_VHOST_POSTCOPY
	struct uffdio_api api_struct;

	if (validate_msg_fds(dev, ctx, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	dev->postcopy_ufd = syscall(__NR_userfaultfd, O_CLOEXEC | O_NONBLOCK);

	if (dev->postcopy_ufd == -1) {
		VHOST_LOG_CONFIG(ERR, "(%s) userfaultfd not available: %s\n",
			dev->ifname, strerror(errno));
		return RTE_VHOST_MSG_RESULT_ERR;
	}
	api_struct.api = UFFD_API;
	api_struct.features = 0;
	if (ioctl(dev->postcopy_ufd, UFFDIO_API, &api_struct)) {
		VHOST_LOG_CONFIG(ERR, "(%s) UFFDIO_API ioctl failure: %s\n",
			dev->ifname, strerror(errno));
		close(dev->postcopy_ufd);
		dev->postcopy_ufd = -1;
		return RTE_VHOST_MSG_RESULT_ERR;
	}
	ctx->fds[0] = dev->postcopy_ufd;
	ctx->fd_num = 1;

	return RTE_VHOST_MSG_RESULT_REPLY;
#else
	dev->postcopy_ufd = -1;
	ctx->fd_num = 0;

	return RTE_VHOST_MSG_RESULT_ERR;
#endif
}