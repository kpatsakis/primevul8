static ssize_t ucma_create_id(struct ucma_file *file, const char __user *inbuf,
			      int in_len, int out_len)
{
	struct rdma_ucm_create_id cmd;
	struct rdma_ucm_create_id_resp resp;
	struct ucma_context *ctx;
	struct rdma_cm_id *cm_id;
	enum ib_qp_type qp_type;
	int ret;

	if (out_len < sizeof(resp))
		return -ENOSPC;

	if (copy_from_user(&cmd, inbuf, sizeof(cmd)))
		return -EFAULT;

	ret = ucma_get_qp_type(&cmd, &qp_type);
	if (ret)
		return ret;

	ctx = ucma_alloc_ctx(file);
	if (!ctx)
		return -ENOMEM;

	ctx->uid = cmd.uid;
	cm_id = __rdma_create_id(current->nsproxy->net_ns,
				 ucma_event_handler, ctx, cmd.ps, qp_type, NULL);
	if (IS_ERR(cm_id)) {
		ret = PTR_ERR(cm_id);
		goto err1;
	}
	ctx->cm_id = cm_id;

	resp.id = ctx->id;
	if (copy_to_user(u64_to_user_ptr(cmd.response),
			 &resp, sizeof(resp))) {
		xa_erase(&ctx_table, ctx->id);
		__destroy_id(ctx);
		return -EFAULT;
	}

	mutex_lock(&file->mut);
	ucma_finish_ctx(ctx);
	mutex_unlock(&file->mut);
	return 0;

err1:
	xa_erase(&ctx_table, ctx->id);
	kfree(ctx);
	return ret;
}