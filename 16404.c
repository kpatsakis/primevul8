static void io_cqring_ev_posted_iopoll(struct io_ring_ctx *ctx)
{
	if (unlikely(ctx->off_timeout_used || ctx->drain_active ||
		     ctx->has_evfd))
		__io_commit_cqring_flush(ctx);

	if (ctx->flags & IORING_SETUP_SQPOLL)
		io_cqring_wake(ctx);
}