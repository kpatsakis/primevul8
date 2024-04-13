static bool io_flush_cached_reqs(struct io_ring_ctx *ctx)
{
	struct io_submit_state *state = &ctx->submit_state;

	/*
	 * If we have more than a batch's worth of requests in our IRQ side
	 * locked cache, grab the lock and move them over to our submission
	 * side cache.
	 */
	if (READ_ONCE(ctx->locked_free_nr) > IO_COMPL_BATCH)
		io_flush_cached_locked_reqs(ctx, state);
	return !!state->free_list.next;
}