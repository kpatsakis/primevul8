
static inline bool io_should_wake(struct io_wait_queue *iowq)
{
	struct io_ring_ctx *ctx = iowq->ctx;
	int dist = ctx->cached_cq_tail - (int) iowq->cq_tail;

	/*
	 * Wake up if we have enough events, or if a timeout occurred since we
	 * started waiting. For timeouts, we always want to return to userspace,
	 * regardless of event count.
	 */
	return dist >= 0 || atomic_read(&ctx->cq_timeouts) != iowq->nr_timeouts;