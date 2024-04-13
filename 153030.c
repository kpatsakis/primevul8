vhost_user_net_set_mtu(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;

	if (validate_msg_fds(dev, ctx, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	if (ctx->msg.payload.u64 < VIRTIO_MIN_MTU ||
			ctx->msg.payload.u64 > VIRTIO_MAX_MTU) {
		VHOST_LOG_CONFIG(ERR, "(%s) invalid MTU size (%"PRIu64")\n",
				dev->ifname, ctx->msg.payload.u64);

		return RTE_VHOST_MSG_RESULT_ERR;
	}

	dev->mtu = ctx->msg.payload.u64;

	return RTE_VHOST_MSG_RESULT_OK;
}