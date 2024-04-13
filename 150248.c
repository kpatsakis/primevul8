static ssize_t ucma_process_join(struct ucma_file *file,
				 struct rdma_ucm_join_mcast *cmd,  int out_len)
{
	struct rdma_ucm_create_id_resp resp;
	struct ucma_context *ctx;
	struct ucma_multicast *mc;
	struct sockaddr *addr;
	int ret;
	u8 join_state;

	if (out_len < sizeof(resp))
		return -ENOSPC;

	addr = (struct sockaddr *) &cmd->addr;
	if (cmd->addr_size != rdma_addr_size(addr))
		return -EINVAL;

	if (cmd->join_flags == RDMA_MC_JOIN_FLAG_FULLMEMBER)
		join_state = BIT(FULLMEMBER_JOIN);
	else if (cmd->join_flags == RDMA_MC_JOIN_FLAG_SENDONLY_FULLMEMBER)
		join_state = BIT(SENDONLY_FULLMEMBER_JOIN);
	else
		return -EINVAL;

	ctx = ucma_get_ctx_dev(file, cmd->id);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	mc = kzalloc(sizeof(*mc), GFP_KERNEL);
	if (!mc) {
		ret = -ENOMEM;
		goto err_put_ctx;
	}

	mc->ctx = ctx;
	mc->join_state = join_state;
	mc->uid = cmd->uid;
	memcpy(&mc->addr, addr, cmd->addr_size);

	if (xa_alloc(&multicast_table, &mc->id, NULL, xa_limit_32b,
		     GFP_KERNEL)) {
		ret = -ENOMEM;
		goto err_free_mc;
	}

	mutex_lock(&ctx->mutex);
	ret = rdma_join_multicast(ctx->cm_id, (struct sockaddr *)&mc->addr,
				  join_state, mc);
	mutex_unlock(&ctx->mutex);
	if (ret)
		goto err_xa_erase;

	resp.id = mc->id;
	if (copy_to_user(u64_to_user_ptr(cmd->response),
			 &resp, sizeof(resp))) {
		ret = -EFAULT;
		goto err_leave_multicast;
	}

	xa_store(&multicast_table, mc->id, mc, 0);

	ucma_put_ctx(ctx);
	return 0;

err_leave_multicast:
	mutex_lock(&ctx->mutex);
	rdma_leave_multicast(ctx->cm_id, (struct sockaddr *) &mc->addr);
	mutex_unlock(&ctx->mutex);
	ucma_cleanup_mc_events(mc);
err_xa_erase:
	xa_erase(&multicast_table, mc->id);
err_free_mc:
	kfree(mc);
err_put_ctx:
	ucma_put_ctx(ctx);
	return ret;
}