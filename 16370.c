static inline void io_cqring_wake(struct io_ring_ctx *ctx)
{
	/*
	 * wake_up_all() may seem excessive, but io_wake_function() and
	 * io_should_wake() handle the termination of the loop and only
	 * wake as many waiters as we need to.
	 */
	if (wq_has_sleeper(&ctx->cq_wait))
		wake_up_all(&ctx->cq_wait);
}