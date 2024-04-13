vhost_user_set_postcopy_listen(struct virtio_net **pdev,
			struct vhu_msg_context *ctx __rte_unused,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;

	if (validate_msg_fds(dev, ctx, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	if (dev->mem && dev->mem->nregions) {
		VHOST_LOG_CONFIG(ERR, "(%s) regions already registered at postcopy-listen\n",
				dev->ifname);
		return RTE_VHOST_MSG_RESULT_ERR;
	}
	dev->postcopy_listening = 1;

	return RTE_VHOST_MSG_RESULT_OK;
}