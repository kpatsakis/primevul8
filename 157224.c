enum dc_status dce110_validate_global(
		struct dc *dc,
		struct dc_state *context)
{
	if (!dce110_validate_surface_sets(context))
		return DC_FAIL_SURFACE_VALIDATE;

	return DC_OK;
}