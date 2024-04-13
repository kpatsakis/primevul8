static int ucma_set_option_id(struct ucma_context *ctx, int optname,
			      void *optval, size_t optlen)
{
	int ret = 0;

	switch (optname) {
	case RDMA_OPTION_ID_TOS:
		if (optlen != sizeof(u8)) {
			ret = -EINVAL;
			break;
		}
		rdma_set_service_type(ctx->cm_id, *((u8 *) optval));
		break;
	case RDMA_OPTION_ID_REUSEADDR:
		if (optlen != sizeof(int)) {
			ret = -EINVAL;
			break;
		}
		ret = rdma_set_reuseaddr(ctx->cm_id, *((int *) optval) ? 1 : 0);
		break;
	case RDMA_OPTION_ID_AFONLY:
		if (optlen != sizeof(int)) {
			ret = -EINVAL;
			break;
		}
		ret = rdma_set_afonly(ctx->cm_id, *((int *) optval) ? 1 : 0);
		break;
	case RDMA_OPTION_ID_ACK_TIMEOUT:
		if (optlen != sizeof(u8)) {
			ret = -EINVAL;
			break;
		}
		ret = rdma_set_ack_timeout(ctx->cm_id, *((u8 *)optval));
		break;
	default:
		ret = -ENOSYS;
	}

	return ret;
}