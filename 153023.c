vhost_user_set_protocol_features(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
	uint64_t protocol_features = ctx->msg.payload.u64;
	uint64_t slave_protocol_features = 0;

	if (validate_msg_fds(dev, ctx, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	rte_vhost_driver_get_protocol_features(dev->ifname,
			&slave_protocol_features);
	if (protocol_features & ~slave_protocol_features) {
		VHOST_LOG_CONFIG(ERR, "(%s) received invalid protocol features.\n", dev->ifname);
		return RTE_VHOST_MSG_RESULT_ERR;
	}

	dev->protocol_features = protocol_features;
	VHOST_LOG_CONFIG(INFO, "(%s) negotiated Vhost-user protocol features: 0x%" PRIx64 "\n",
		dev->ifname, dev->protocol_features);

	return RTE_VHOST_MSG_RESULT_OK;
}