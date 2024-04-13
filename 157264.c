static struct output_pixel_processor *dcn10_opp_create(
	struct dc_context *ctx, uint32_t inst)
{
	struct dcn10_opp *opp =
		kzalloc(sizeof(struct dcn10_opp), GFP_KERNEL);

	if (!opp) {
		BREAK_TO_DEBUGGER();
		return NULL;
	}

	dcn10_opp_construct(opp, ctx, inst,
			&opp_regs[inst], &opp_shift, &opp_mask);
	return &opp->base;
}