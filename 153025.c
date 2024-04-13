static int vhost_user_set_log_fd(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;

	if (validate_msg_fds(dev, ctx, 1) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	close(ctx->fds[0]);
	VHOST_LOG_CONFIG(INFO, "(%s) not implemented.\n", dev->ifname);

	return RTE_VHOST_MSG_RESULT_OK;
}