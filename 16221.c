 */
static const struct io_uring_sqe *io_get_sqe(struct io_ring_ctx *ctx)
{
	unsigned head, mask = ctx->sq_entries - 1;
	unsigned sq_idx = ctx->cached_sq_head++ & mask;

	/*
	 * The cached sq head (or cq tail) serves two purposes:
	 *
	 * 1) allows us to batch the cost of updating the user visible
	 *    head updates.
	 * 2) allows the kernel side to track the head on its own, even
	 *    though the application is the one updating it.
	 */
	head = READ_ONCE(ctx->sq_array[sq_idx]);
	if (likely(head < ctx->sq_entries))
		return &ctx->sq_sqes[head];

	/* drop invalid entries */
	ctx->cq_extra--;
	WRITE_ONCE(ctx->rings->sq_dropped,
		   READ_ONCE(ctx->rings->sq_dropped) + 1);
	return NULL;