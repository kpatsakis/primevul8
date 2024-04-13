static ssize_t ucma_accept(struct ucma_file *file, const char __user *inbuf,
			   int in_len, int out_len)
{
	struct rdma_ucm_accept cmd;
	struct rdma_conn_param conn_param;
	struct rdma_ucm_ece ece = {};
	struct ucma_context *ctx;
	size_t in_size;
	int ret;

	if (in_len < offsetofend(typeof(cmd), reserved))
		return -EINVAL;
	in_size = min_t(size_t, in_len, sizeof(cmd));
	if (copy_from_user(&cmd, inbuf, in_size))
		return -EFAULT;

	ctx = ucma_get_ctx_dev(file, cmd.id);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	if (offsetofend(typeof(cmd), ece) <= in_size) {
		ece.vendor_id = cmd.ece.vendor_id;
		ece.attr_mod = cmd.ece.attr_mod;
	}

	if (cmd.conn_param.valid) {
		ucma_copy_conn_param(ctx->cm_id, &conn_param, &cmd.conn_param);
		mutex_lock(&ctx->mutex);
		rdma_lock_handler(ctx->cm_id);
		ret = __rdma_accept_ece(ctx->cm_id, &conn_param, NULL, &ece);
		if (!ret) {
			/* The uid must be set atomically with the handler */
			ctx->uid = cmd.uid;
		}
		rdma_unlock_handler(ctx->cm_id);
		mutex_unlock(&ctx->mutex);
	} else {
		mutex_lock(&ctx->mutex);
		rdma_lock_handler(ctx->cm_id);
		ret = __rdma_accept_ece(ctx->cm_id, NULL, NULL, &ece);
		rdma_unlock_handler(ctx->cm_id);
		mutex_unlock(&ctx->mutex);
	}
	ucma_put_ctx(ctx);
	return ret;
}