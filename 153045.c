vhost_user_postcopy_end(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;

	if (validate_msg_fds(dev, ctx, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	dev->postcopy_listening = 0;
	if (dev->postcopy_ufd >= 0) {
		close(dev->postcopy_ufd);
		dev->postcopy_ufd = -1;
	}

	ctx->msg.payload.u64 = 0;
	ctx->msg.size = sizeof(ctx->msg.payload.u64);
	ctx->fd_num = 0;

	return RTE_VHOST_MSG_RESULT_REPLY;
}