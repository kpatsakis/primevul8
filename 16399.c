static inline void io_cqring_ev_posted(struct io_ring_ctx *ctx)
{
	if (unlikely(ctx->off_timeout_used || ctx->drain_active ||
		     ctx->has_evfd))
		__io_commit_cqring_flush(ctx);

	io_cqring_wake(ctx);
}