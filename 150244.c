static ssize_t ucma_disconnect(struct ucma_file *file, const char __user *inbuf,
			       int in_len, int out_len)
{
	struct rdma_ucm_disconnect cmd;
	struct ucma_context *ctx;
	int ret;

	if (copy_from_user(&cmd, inbuf, sizeof(cmd)))
		return -EFAULT;

	ctx = ucma_get_ctx_dev(file, cmd.id);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	mutex_lock(&ctx->mutex);
	ret = rdma_disconnect(ctx->cm_id);
	mutex_unlock(&ctx->mutex);
	ucma_put_ctx(ctx);
	return ret;
}