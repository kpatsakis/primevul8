static int ucma_set_ib_path(struct ucma_context *ctx,
			    struct ib_path_rec_data *path_data, size_t optlen)
{
	struct sa_path_rec sa_path;
	struct rdma_cm_event event;
	int ret;

	if (optlen % sizeof(*path_data))
		return -EINVAL;

	for (; optlen; optlen -= sizeof(*path_data), path_data++) {
		if (path_data->flags == (IB_PATH_GMP | IB_PATH_PRIMARY |
					 IB_PATH_BIDIRECTIONAL))
			break;
	}

	if (!optlen)
		return -EINVAL;

	if (!ctx->cm_id->device)
		return -EINVAL;

	memset(&sa_path, 0, sizeof(sa_path));

	sa_path.rec_type = SA_PATH_REC_TYPE_IB;
	ib_sa_unpack_path(path_data->path_rec, &sa_path);

	if (rdma_cap_opa_ah(ctx->cm_id->device, ctx->cm_id->port_num)) {
		struct sa_path_rec opa;

		sa_convert_path_ib_to_opa(&opa, &sa_path);
		mutex_lock(&ctx->mutex);
		ret = rdma_set_ib_path(ctx->cm_id, &opa);
		mutex_unlock(&ctx->mutex);
	} else {
		mutex_lock(&ctx->mutex);
		ret = rdma_set_ib_path(ctx->cm_id, &sa_path);
		mutex_unlock(&ctx->mutex);
	}
	if (ret)
		return ret;

	memset(&event, 0, sizeof event);
	event.event = RDMA_CM_EVENT_ROUTE_RESOLVED;
	return ucma_event_handler(ctx->cm_id, &event);
}