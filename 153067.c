send_vhost_reply(struct virtio_net *dev, int sockfd, struct vhu_msg_context *ctx)
{
	if (!ctx)
		return 0;

	ctx->msg.flags &= ~VHOST_USER_VERSION_MASK;
	ctx->msg.flags &= ~VHOST_USER_NEED_REPLY;
	ctx->msg.flags |= VHOST_USER_VERSION;
	ctx->msg.flags |= VHOST_USER_REPLY_MASK;

	return send_vhost_message(dev, sockfd, ctx);
}