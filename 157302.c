static struct pipe_ctx *dcn10_acquire_idle_pipe_for_layer(
		struct dc_state *context,
		const struct resource_pool *pool,
		struct dc_stream_state *stream)
{
	struct resource_context *res_ctx = &context->res_ctx;
	struct pipe_ctx *head_pipe = resource_get_head_pipe_for_stream(res_ctx, stream);
	struct pipe_ctx *idle_pipe = find_idle_secondary_pipe(res_ctx, pool, head_pipe);

	if (!head_pipe) {
		ASSERT(0);
		return NULL;
	}

	if (!idle_pipe)
		return NULL;

	idle_pipe->stream = head_pipe->stream;
	idle_pipe->stream_res.tg = head_pipe->stream_res.tg;
	idle_pipe->stream_res.abm = head_pipe->stream_res.abm;
	idle_pipe->stream_res.opp = head_pipe->stream_res.opp;

	idle_pipe->plane_res.hubp = pool->hubps[idle_pipe->pipe_idx];
	idle_pipe->plane_res.ipp = pool->ipps[idle_pipe->pipe_idx];
	idle_pipe->plane_res.dpp = pool->dpps[idle_pipe->pipe_idx];
	idle_pipe->plane_res.mpcc_inst = pool->dpps[idle_pipe->pipe_idx]->inst;

	return idle_pipe;
}