static void ctx_flush_and_put(struct io_ring_ctx *ctx, bool *locked)
{
	if (!ctx)
		return;
	if (*locked) {
		io_submit_flush_completions(ctx);
		mutex_unlock(&ctx->uring_lock);
		*locked = false;
	}
	percpu_ref_put(&ctx->refs);
}