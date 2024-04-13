pdf_copy_pattern_gstate(fz_context *ctx, pdf_gstate *gs, const pdf_gstate *old)
{
	gs->ctm = old->ctm;

	pdf_drop_font(ctx, gs->text.font);
	gs->text.font = pdf_keep_font(ctx, old->text.font);

	pdf_drop_xobject(ctx, gs->softmask);
	gs->softmask = pdf_keep_xobject(ctx, old->softmask);

	fz_drop_stroke_state(ctx, gs->stroke_state);
	gs->stroke_state = fz_keep_stroke_state(ctx, old->stroke_state);
}