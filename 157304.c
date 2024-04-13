static struct pp_smu_funcs *dcn10_pp_smu_create(struct dc_context *ctx)
{
	struct pp_smu_funcs *pp_smu = kzalloc(sizeof(*pp_smu), GFP_KERNEL);

	if (!pp_smu)
		return pp_smu;

	dm_pp_get_funcs(ctx, pp_smu);
	return pp_smu;
}