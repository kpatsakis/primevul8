pdf_set_pattern(fz_context *ctx, pdf_run_processor *pr, int what, pdf_pattern *pat, float *v)
{
	pdf_gstate *gs;
	pdf_material *mat;

	gs = pdf_flush_text(ctx, pr);

	mat = what == PDF_FILL ? &gs->fill : &gs->stroke;

	pdf_drop_pattern(ctx, mat->pattern);
	mat->pattern = NULL;

	mat->kind = PDF_MAT_PATTERN;
	if (pat)
		mat->pattern = pdf_keep_pattern(ctx, pat);

	if (v)
		pdf_set_color(ctx, pr, what, v);

	mat->gstate_num = pr->gparent;
}