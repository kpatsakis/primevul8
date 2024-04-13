 */
static void io_submit_state_end(struct io_ring_ctx *ctx)
{
	struct io_submit_state *state = &ctx->submit_state;

	if (state->link.head)
		io_queue_sqe(state->link.head);
	/* flush only after queuing links as they can generate completions */
	io_submit_flush_completions(ctx);
	if (state->plug_started)
		blk_finish_plug(&state->plug);