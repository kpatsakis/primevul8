static ssize_t ucma_reject(struct ucma_file *file, const char __user *inbuf,
			   int in_len, int out_len)
{
	struct rdma_ucm_reject cmd;
	struct ucma_context *ctx;
	int ret;

	if (copy_from_user(&cmd, inbuf, sizeof(cmd)))
		return -EFAULT;

	if (!cmd.reason)
		cmd.reason = IB_CM_REJ_CONSUMER_DEFINED;

	switch (cmd.reason) {
	case IB_CM_REJ_CONSUMER_DEFINED:
	case IB_CM_REJ_VENDOR_OPTION_NOT_SUPPORTED:
		break;
	default:
		return -EINVAL;
	}

	ctx = ucma_get_ctx_dev(file, cmd.id);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	mutex_lock(&ctx->mutex);
	ret = rdma_reject(ctx->cm_id, cmd.private_data, cmd.private_data_len,
			  cmd.reason);
	mutex_unlock(&ctx->mutex);
	ucma_put_ctx(ctx);
	return ret;
}