pdf_drop_run_processor(fz_context *ctx, pdf_processor *proc)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;

	while (pr->gtop)
		pdf_grestore(ctx, pr);

	pdf_drop_material(ctx, &pr->gstate[0].fill);
	pdf_drop_material(ctx, &pr->gstate[0].stroke);
	pdf_drop_font(ctx, pr->gstate[0].text.font);
	pdf_drop_xobject(ctx, pr->gstate[0].softmask);
	fz_drop_stroke_state(ctx, pr->gstate[0].stroke_state);

	while (pr->gstate[0].clip_depth--)
		fz_pop_clip(ctx, pr->dev);

	fz_drop_path(ctx, pr->path);
	fz_drop_text(ctx, pr->tos.text);

	fz_drop_default_colorspaces(ctx, pr->default_cs);

	fz_free(ctx, pr->gstate);
}