static inline bool io_alloc_req_refill(struct io_ring_ctx *ctx)
{
	if (unlikely(!ctx->submit_state.free_list.next))
		return __io_alloc_req_refill(ctx);
	return true;
}