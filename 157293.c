static struct dce_hwseq *dce121_hwseq_create(
	struct dc_context *ctx)
{
	struct dce_hwseq *hws = kzalloc(sizeof(struct dce_hwseq), GFP_KERNEL);

	if (hws) {
		hws->ctx = ctx;
		hws->regs = &dce121_hwseq_reg;
		hws->shifts = &dce121_hwseq_shift;
		hws->masks = &dce121_hwseq_mask;
	}
	return hws;
}