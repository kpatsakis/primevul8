static inline void io_submit_flush_completions(struct io_ring_ctx *ctx)
{
	if (!wq_list_empty(&ctx->submit_state.compl_reqs))
		__io_submit_flush_completions(ctx);
}