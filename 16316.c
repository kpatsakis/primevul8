
static inline void io_ring_set_wakeup_flag(struct io_ring_ctx *ctx)
{
	/* Tell userspace we may need a wakeup call */
	spin_lock(&ctx->completion_lock);
	WRITE_ONCE(ctx->rings->sq_flags,
		   ctx->rings->sq_flags | IORING_SQ_NEED_WAKEUP);
	spin_unlock(&ctx->completion_lock);