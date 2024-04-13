static inline void ctx_commit_and_unlock(struct io_ring_ctx *ctx)
{
	io_commit_cqring(ctx);
	spin_unlock(&ctx->completion_lock);
	io_cqring_ev_posted(ctx);
}