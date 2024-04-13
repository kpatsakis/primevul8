static inline struct io_uring_cqe *io_get_cqe(struct io_ring_ctx *ctx)
{
	struct io_rings *rings = ctx->rings;
	unsigned tail, mask = ctx->cq_entries - 1;

	/*
	 * writes to the cq entry need to come after reading head; the
	 * control dependency is enough as we're using WRITE_ONCE to
	 * fill the cq entry
	 */
	if (__io_cqring_events(ctx) == ctx->cq_entries)
		return NULL;

	tail = ctx->cached_cq_tail++;
	return &rings->cqes[tail & mask];
}