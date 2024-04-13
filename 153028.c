send_vhost_message(struct virtio_net *dev, int sockfd, struct vhu_msg_context *ctx)
{
	if (!ctx)
		return 0;

	return send_fd_message(dev->ifname, sockfd, (char *)&ctx->msg,
		VHOST_USER_HDR_SIZE + ctx->msg.size, ctx->fds, ctx->fd_num);
}