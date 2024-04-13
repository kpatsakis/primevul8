static struct dce_hwseq *dce110_hwseq_create(
	struct dc_context *ctx)
{
	struct dce_hwseq *hws = kzalloc(sizeof(struct dce_hwseq), GFP_KERNEL);

	if (hws) {
		hws->ctx = ctx;
		hws->regs = ASIC_REV_IS_STONEY(ctx->asic_id.hw_internal_rev) ?
				&hwseq_stoney_reg : &hwseq_cz_reg;
		hws->shifts = &hwseq_shift;
		hws->masks = &hwseq_mask;
		hws->wa.blnd_crtc_trigger = true;
	}
	return hws;
}