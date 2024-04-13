pdf_keep_gstate(fz_context *ctx, pdf_gstate *gs)
{
	pdf_keep_material(ctx, &gs->stroke);
	pdf_keep_material(ctx, &gs->fill);
	if (gs->text.font)
		pdf_keep_font(ctx, gs->text.font);
	if (gs->softmask)
		pdf_keep_xobject(ctx, gs->softmask);
	if (gs->softmask_resources)
		pdf_keep_obj(ctx, gs->softmask_resources);
	fz_keep_stroke_state(ctx, gs->stroke_state);
}