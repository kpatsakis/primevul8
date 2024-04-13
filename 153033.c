read_vhost_message(struct virtio_net *dev, int sockfd, struct  vhu_msg_context *ctx)
{
	int ret;

	ret = read_fd_message(dev->ifname, sockfd, (char *)&ctx->msg, VHOST_USER_HDR_SIZE,
		ctx->fds, VHOST_MEMORY_MAX_NREGIONS, &ctx->fd_num);
	if (ret <= 0) {
		return ret;
	} else if (ret != VHOST_USER_HDR_SIZE) {
		VHOST_LOG_CONFIG(ERR, "(%s) Unexpected header size read\n", dev->ifname);
		close_msg_fds(ctx);
		return -1;
	}

	if (ctx->msg.size) {
		if (ctx->msg.size > sizeof(ctx->msg.payload)) {
			VHOST_LOG_CONFIG(ERR, "(%s) invalid msg size: %d\n",
					dev->ifname, ctx->msg.size);
			return -1;
		}
		ret = read(sockfd, &ctx->msg.payload, ctx->msg.size);
		if (ret <= 0)
			return ret;
		if (ret != (int)ctx->msg.size) {
			VHOST_LOG_CONFIG(ERR, "(%s) read control message failed\n", dev->ifname);
			return -1;
		}
	}

	return ret;
}