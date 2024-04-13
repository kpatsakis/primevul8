static void io_flush_cached_locked_reqs(struct io_ring_ctx *ctx,
					struct io_submit_state *state)
{
	spin_lock(&ctx->completion_lock);
	wq_list_splice(&ctx->locked_free_list, &state->free_list);
	ctx->locked_free_nr = 0;
	spin_unlock(&ctx->completion_lock);
}