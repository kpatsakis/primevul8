static enum dc_status build_mapped_resource(
		const struct dc  *dc,
		struct dc_state *context,
		struct dc_stream_state *stream)
{
	struct pipe_ctx *pipe_ctx = resource_get_head_pipe_for_stream(&context->res_ctx, stream);

	if (!pipe_ctx)
		return DC_ERROR_UNEXPECTED;

	dce110_resource_build_pipe_hw_param(pipe_ctx);

	resource_build_info_frame(pipe_ctx);

	return DC_OK;
}