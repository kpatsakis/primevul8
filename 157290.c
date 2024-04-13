static enum dc_status dcn10_validate_plane(const struct dc_plane_state *plane_state, struct dc_caps *caps)
{
	if (plane_state->format >= SURFACE_PIXEL_FORMAT_VIDEO_BEGIN
			&& caps->max_video_width != 0
			&& plane_state->src_rect.width > caps->max_video_width)
		return DC_FAIL_SURFACE_VALIDATE;

	return DC_OK;
}