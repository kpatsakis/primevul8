validate_msg_fds(struct virtio_net *dev, struct vhu_msg_context *ctx, int expected_fds)
{
	if (ctx->fd_num == expected_fds)
		return 0;

	VHOST_LOG_CONFIG(ERR, "(%s) expect %d FDs for request %s, received %d\n",
		dev->ifname, expected_fds,
		vhost_message_str[ctx->msg.request.master],
		ctx->fd_num);

	close_msg_fds(ctx);

	return -1;
}