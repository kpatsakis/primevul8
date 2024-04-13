static struct hubbub *dcn10_hubbub_create(struct dc_context *ctx)
{
	struct dcn10_hubbub *dcn10_hubbub = kzalloc(sizeof(struct dcn10_hubbub),
					  GFP_KERNEL);

	if (!dcn10_hubbub)
		return NULL;

	hubbub1_construct(&dcn10_hubbub->base, ctx,
			&hubbub_reg,
			&hubbub_shift,
			&hubbub_mask);

	return &dcn10_hubbub->base;
}