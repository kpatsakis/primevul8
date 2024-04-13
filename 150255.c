static ssize_t ucma_query_addr(struct ucma_context *ctx,
			       void __user *response, int out_len)
{
	struct rdma_ucm_query_addr_resp resp;
	struct sockaddr *addr;
	int ret = 0;

	if (out_len < offsetof(struct rdma_ucm_query_addr_resp, ibdev_index))
		return -ENOSPC;

	memset(&resp, 0, sizeof resp);

	addr = (struct sockaddr *) &ctx->cm_id->route.addr.src_addr;
	resp.src_size = rdma_addr_size(addr);
	memcpy(&resp.src_addr, addr, resp.src_size);

	addr = (struct sockaddr *) &ctx->cm_id->route.addr.dst_addr;
	resp.dst_size = rdma_addr_size(addr);
	memcpy(&resp.dst_addr, addr, resp.dst_size);

	ucma_query_device_addr(ctx->cm_id, &resp);

	if (copy_to_user(response, &resp, min_t(size_t, out_len, sizeof(resp))))
		ret = -EFAULT;

	return ret;
}