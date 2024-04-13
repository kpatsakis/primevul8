static struct hubp *dcn10_hubp_create(
	struct dc_context *ctx,
	uint32_t inst)
{
	struct dcn10_hubp *hubp1 =
		kzalloc(sizeof(struct dcn10_hubp), GFP_KERNEL);

	if (!hubp1)
		return NULL;

	dcn10_hubp_construct(hubp1, ctx, inst,
			     &hubp_regs[inst], &hubp_shift, &hubp_mask);
	return &hubp1->base;
}