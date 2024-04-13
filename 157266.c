static struct dpp *dcn10_dpp_create(
	struct dc_context *ctx,
	uint32_t inst)
{
	struct dcn10_dpp *dpp =
		kzalloc(sizeof(struct dcn10_dpp), GFP_KERNEL);

	if (!dpp)
		return NULL;

	dpp1_construct(dpp, ctx, inst,
		       &tf_regs[inst], &tf_shift, &tf_mask);
	return &dpp->base;
}