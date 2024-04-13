static ssize_t ucma_notify(struct ucma_file *file, const char __user *inbuf,
			   int in_len, int out_len)
{
	struct rdma_ucm_notify cmd;
	struct ucma_context *ctx;
	int ret = -EINVAL;

	if (copy_from_user(&cmd, inbuf, sizeof(cmd)))
		return -EFAULT;

	ctx = ucma_get_ctx(file, cmd.id);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	mutex_lock(&ctx->mutex);
	if (ctx->cm_id->device)
		ret = rdma_notify(ctx->cm_id, (enum ib_event_type)cmd.event);
	mutex_unlock(&ctx->mutex);

	ucma_put_ctx(ctx);
	return ret;
}