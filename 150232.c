static ssize_t ucma_resolve_ip(struct ucma_file *file,
			       const char __user *inbuf,
			       int in_len, int out_len)
{
	struct rdma_ucm_resolve_ip cmd;
	struct ucma_context *ctx;
	int ret;

	if (copy_from_user(&cmd, inbuf, sizeof(cmd)))
		return -EFAULT;

	if ((cmd.src_addr.sin6_family && !rdma_addr_size_in6(&cmd.src_addr)) ||
	    !rdma_addr_size_in6(&cmd.dst_addr))
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