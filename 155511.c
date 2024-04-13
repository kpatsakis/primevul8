pdf_drop_gstate(fz_context *ctx, pdf_gstate *gs)
{
	pdf_drop_material(ctx, &gs->stroke);
	pdf_drop_material(ctx, &gs->fill);
	pdf_drop_font(ctx, gs->text.font);
	pdf_drop_xobject(ctx, gs->softmask);
	pdf_drop_obj(ctx, gs->softmask_resources);
	fz_drop_stroke_state(ctx, gs->stroke_state);
}