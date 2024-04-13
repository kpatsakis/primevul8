vhost_user_set_vring_enable(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
	bool enable = !!ctx->msg.payload.state.num;
	int index = (int)ctx->msg.payload.state.index;

	if (validate_msg_fds(dev, ctx, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	VHOST_LOG_CONFIG(INFO, "(%s) set queue enable: %d to qp idx: %d\n",
			dev->ifname, enable, index);

	if (enable && dev->virtqueue[index]->async) {
		if (dev->virtqueue[index]->async->pkts_inflight_n) {
			VHOST_LOG_CONFIG(ERR,
				"(%s) failed to enable vring. Inflight packets must be completed first\n",
				dev->ifname);
			return RTE_VHOST_MSG_RESULT_ERR;
		}
	}

	dev->virtqueue[index]->enabled = enable;

	return RTE_VHOST_MSG_RESULT_OK;
}