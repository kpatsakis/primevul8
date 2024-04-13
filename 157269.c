enum dc_status resource_map_phy_clock_resources(
		const struct dc *dc,
		struct dc_state *context,
		struct dc_stream_state *stream)
{

	/* acquire new resources */
	struct pipe_ctx *pipe_ctx = resource_get_head_pipe_for_stream(
			&context->res_ctx, stream);

	if (!pipe_ctx)
		return DC_ERROR_UNEXPECTED;

	if (dc_is_dp_signal(pipe_ctx->stream->signal)
		|| dc_is_virtual_signal(pipe_ctx->stream->signal))
		pipe_ctx->clock_source =
				dc->res_pool->dp_clock_source;
	else
		pipe_ctx->clock_source = find_matching_pll(
			&context->res_ctx, dc->res_pool,
			stream);

	if (pipe_ctx->clock_source == NULL)
		return DC_NO_CLOCK_SOURCE_RESOURCE;

	resource_reference_clock_source(
		&context->res_ctx,
		dc->res_pool,
		pipe_ctx->clock_source);

	return DC_OK;
}