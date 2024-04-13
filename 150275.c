static ssize_t ucma_query_route(struct ucma_file *file,
				const char __user *inbuf,
				int in_len, int out_len)
{
	struct rdma_ucm_query cmd;
	struct rdma_ucm_query_route_resp resp;
	struct ucma_context *ctx;
	struct sockaddr *addr;
	int ret = 0;

	if (out_len < offsetof(struct rdma_ucm_query_route_resp, ibdev_index))
		return -ENOSPC;

	if (copy_from_user(&cmd, inbuf, sizeof(cmd)))
		return -EFAULT;

	ctx = ucma_get_ctx(file, cmd.id);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	mutex_lock(&ctx->mutex);
	memset(&resp, 0, sizeof resp);
	addr = (struct sockaddr *) &ctx->cm_id->route.addr.src_addr;
	memcpy(&resp.src_addr, addr, addr->sa_family == AF_INET ?
				     sizeof(struct sockaddr_in) :
				     sizeof(struct sockaddr_in6));
	addr = (struct sockaddr *) &ctx->cm_id->route.addr.dst_addr;
	memcpy(&resp.dst_addr, addr, addr->sa_family == AF_INET ?
				     sizeof(struct sockaddr_in) :
				     sizeof(struct sockaddr_in6));
	if (!ctx->cm_id->device)
		goto out;

	resp.node_guid = (__force __u64) ctx->cm_id->device->node_guid;
	resp.ibdev_index = ctx->cm_id->device->index;
	resp.port_num = ctx->cm_id->port_num;

	if (rdma_cap_ib_sa(ctx->cm_id->device, ctx->cm_id->port_num))
		ucma_copy_ib_route(&resp, &ctx->cm_id->route);
	else if (rdma_protocol_roce(ctx->cm_id->device, ctx->cm_id->port_num))
		ucma_copy_iboe_route(&resp, &ctx->cm_id->route);
	else if (rdma_protocol_iwarp(ctx->cm_id->device, ctx->cm_id->port_num))
		ucma_copy_iw_route(&resp, &ctx->cm_id->route);

out:
	mutex_unlock(&ctx->mutex);
	if (copy_to_user(u64_to_user_ptr(cmd.response), &resp,
			 min_t(size_t, out_len, sizeof(resp))))
		ret = -EFAULT;

	ucma_put_ctx(ctx);
	return ret;
}