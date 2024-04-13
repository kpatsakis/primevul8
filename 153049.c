vhost_user_get_features(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
	uint64_t features = 0;

	if (validate_msg_fds(dev, ctx, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	rte_vhost_driver_get_features(dev->ifname, &features);

	ctx->msg.payload.u64 = features;
	ctx->msg.size = sizeof(ctx->msg.payload.u64);
	ctx->fd_num = 0;

	return RTE_VHOST_MSG_RESULT_REPLY;
}