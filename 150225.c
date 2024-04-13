static ssize_t ucma_bind_ip(struct ucma_file *file, const char __user *inbuf,
			      int in_len, int out_len)
{
	struct rdma_ucm_bind_ip cmd;
	struct ucma_context *ctx;
	int ret;

	if (copy_from_user(&cmd, inbuf, sizeof(cmd)))
		return -EFAULT;

	if (!rdma_addr_size_in6(&cmd.addr))
		return -EINVAL;

	ctx = ucma_get_ctx(file, cmd.id);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	mutex_lock(&ctx->mutex);
	ret = rdma_bind_addr(ctx->cm_id, (struct sockaddr *) &cmd.addr);
	mutex_unlock(&ctx->mutex);

	ucma_put_ctx(ctx);
	return ret;
}