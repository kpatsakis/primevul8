static noinline bool io_fill_cqe_aux(struct io_ring_ctx *ctx, u64 user_data,
				     s32 res, u32 cflags)
{
	ctx->cq_extra++;
	trace_io_uring_complete(ctx, NULL, user_data, res, cflags);
	return __io_fill_cqe(ctx, user_data, res, cflags);
}