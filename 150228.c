static ssize_t ucma_resolve_addr(struct ucma_file *file,
				 const char __user *inbuf,
				 int in_len, int out_len)
{
	struct rdma_ucm_resolve_addr cmd;
	struct ucma_context *ctx;
	int ret;

	if (copy_from_user(&cmd, inbuf, sizeof(cmd)))
		return -EFAULT;

	if (cmd.reserved ||
	    (cmd.src_size && (cmd.src_size != rdma_addr_size_kss(&cmd.src_addr))) ||
	    !cmd.dst_size || (cmd.dst_size != rdma_addr_size_kss(&cmd.dst_addr)))
		return -EINVAL;

	ctx = ucma_get_ctx(file, cmd.id);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	mutex_lock(&ctx->mutex);
	ret = rdma_resolve_addr(ctx->cm_id, (struct sockaddr *) &cmd.src_addr,
				(struct sockaddr *) &cmd.dst_addr, cmd.timeout_ms);
	mutex_unlock(&ctx->mutex);
	ucma_put_ctx(ctx);
	return ret;
}