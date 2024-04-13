static bool is_surface_pixel_format_supported(struct pipe_ctx *pipe_ctx, unsigned int underlay_idx)
{
	if (pipe_ctx->pipe_idx != underlay_idx)
		return true;
	if (!pipe_ctx->plane_state)
		return false;
	if (pipe_ctx->plane_state->format < SURFACE_PIXEL_FORMAT_VIDEO_BEGIN)
		return false;
	return true;
}