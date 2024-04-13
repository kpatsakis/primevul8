static struct dce_hwseq *dcn10_hwseq_create(
	struct dc_context *ctx)
{
	struct dce_hwseq *hws = kzalloc(sizeof(struct dce_hwseq), GFP_KERNEL);

	if (hws) {
		hws->ctx = ctx;
		hws->regs = &hwseq_reg;
		hws->shifts = &hwseq_shift;
		hws->masks = &hwseq_mask;
		hws->wa.DEGVIDCN10_253 = true;
		hws->wa.false_optc_underflow = true;
		hws->wa.DEGVIDCN10_254 = true;
	}
	return hws;
}