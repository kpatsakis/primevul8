enum dc_status dce110_validate_plane(const struct dc_plane_state *plane_state,
				     struct dc_caps *caps)
{
	if (((plane_state->dst_rect.width * 2) < plane_state->src_rect.width) ||
	    ((plane_state->dst_rect.height * 2) < plane_state->src_rect.height))
		return DC_FAIL_SURFACE_VALIDATE;

	return DC_OK;
}