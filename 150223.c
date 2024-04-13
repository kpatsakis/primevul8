static ssize_t ucma_destroy_id(struct ucma_file *file, const char __user *inbuf,
			       int in_len, int out_len)
{
	struct rdma_ucm_destroy_id cmd;
	struct rdma_ucm_destroy_id_resp resp;
	struct ucma_context *ctx;
	int ret = 0;

	if (out_len < sizeof(resp))
		return -ENOSPC;

	if (copy_from_user(&cmd, inbuf, sizeof(cmd)))
		return -EFAULT;

	xa_lock(&ctx_table);
	ctx = _ucma_find_context(cmd.id, file);
	if (!IS_ERR(ctx))
		__xa_erase(&ctx_table, ctx->id);
	xa_unlock(&ctx_table);

	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	resp.events_reported = __destroy_id(ctx);
	if (copy_to_user(u64_to_user_ptr(cmd.response),
			 &resp, sizeof(resp)))
		ret = -EFAULT;

	return ret;
}