static int ucma_set_option_level(struct ucma_context *ctx, int level,
				 int optname, void *optval, size_t optlen)
{
	int ret;

	switch (level) {
	case RDMA_OPTION_ID:
		mutex_lock(&ctx->mutex);
		ret = ucma_set_option_id(ctx, optname, optval, optlen);
		mutex_unlock(&ctx->mutex);
		break;
	case RDMA_OPTION_IB:
		ret = ucma_set_option_ib(ctx, optname, optval, optlen);
		break;
	default:
		ret = -ENOSYS;
	}

	return ret;
}