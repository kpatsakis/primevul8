enum dc_status dce100_validate_plane(const struct dc_plane_state *plane_state, struct dc_caps *caps)
{

	if (plane_state->format < SURFACE_PIXEL_FORMAT_VIDEO_BEGIN)
		return DC_OK;

	return DC_FAIL_SURFACE_VALIDATE;
}