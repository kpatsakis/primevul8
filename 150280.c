static int ucma_set_option_ib(struct ucma_context *ctx, int optname,
			      void *optval, size_t optlen)
{
	int ret;

	switch (optname) {
	case RDMA_OPTION_IB_PATH:
		ret = ucma_set_ib_path(ctx, optval, optlen);
		break;
	default:
		ret = -ENOSYS;
	}

	return ret;
}