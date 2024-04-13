static ssize_t ucma_query_gid(struct ucma_context *ctx,
			      void __user *response, int out_len)
{
	struct rdma_ucm_query_addr_resp resp;
	struct sockaddr_ib *addr;
	int ret = 0;

	if (out_len < offsetof(struct rdma_ucm_query_addr_resp, ibdev_index))
		return -ENOSPC;

	memset(&resp, 0, sizeof resp);

	ucma_query_device_addr(ctx->cm_id, &resp);

	addr = (struct sockaddr_ib *) &resp.src_addr;
	resp.src_size = sizeof(*addr);
	if (ctx->cm_id->route.addr.src_addr.ss_family == AF_IB) {
		memcpy(addr, &ctx->cm_id->route.addr.src_addr, resp.src_size);
	} else {
		addr->sib_family = AF_IB;
		addr->sib_pkey = (__force __be16) resp.pkey;
		rdma_read_gids(ctx->cm_id, (union ib_gid *)&addr->sib_addr,
			       NULL);
		addr->sib_sid = rdma_get_service_id(ctx->cm_id, (struct sockaddr *)
						    &ctx->cm_id->route.addr.src_addr);
	}

	addr = (struct sockaddr_ib *) &resp.dst_addr;
	resp.dst_size = sizeof(*addr);
	if (ctx->cm_id->route.addr.dst_addr.ss_family == AF_IB) {
		memcpy(addr, &ctx->cm_id->route.addr.dst_addr, resp.dst_size);
	} else {
		addr->sib_family = AF_IB;
		addr->sib_pkey = (__force __be16) resp.pkey;
		rdma_read_gids(ctx->cm_id, NULL,
			       (union ib_gid *)&addr->sib_addr);
		addr->sib_sid = rdma_get_service_id(ctx->cm_id, (struct sockaddr *)
						    &ctx->cm_id->route.addr.dst_addr);
	}

	if (copy_to_user(response, &resp, min_t(size_t, out_len, sizeof(resp))))
		ret = -EFAULT;

	return ret;
}