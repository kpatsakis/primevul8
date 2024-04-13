static int vhost_user_set_vring_err(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
	int expected_fds;

	expected_fds = (ctx->msg.payload.u64 & VHOST_USER_VRING_NOFD_MASK) ? 0 : 1;
	if (validate_msg_fds(dev, ctx, expected_fds) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	if (!(ctx->msg.payload.u64 & VHOST_USER_VRING_NOFD_MASK))
		close(ctx->fds[0]);
	VHOST_LOG_CONFIG(INFO, "(%s) not implemented\n", dev->ifname);

	return RTE_VHOST_MSG_RESULT_OK;
}