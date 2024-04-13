static void __io_commit_cqring_flush(struct io_ring_ctx *ctx)
{
	if (ctx->off_timeout_used || ctx->drain_active) {
		spin_lock(&ctx->completion_lock);
		if (ctx->off_timeout_used)
			io_flush_timeouts(ctx);
		if (ctx->drain_active)
			io_queue_deferred(ctx);
		io_commit_cqring(ctx);
		spin_unlock(&ctx->completion_lock);
	}
	if (ctx->has_evfd)
		io_eventfd_signal(ctx);
}