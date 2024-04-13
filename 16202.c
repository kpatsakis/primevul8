static inline void io_tw_lock(struct io_ring_ctx *ctx, bool *locked)
{
	if (!*locked) {
		mutex_lock(&ctx->uring_lock);
		*locked = true;
	}
}