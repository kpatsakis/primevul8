pdf_unset_pattern(fz_context *ctx, pdf_run_processor *pr, int what)
{
	pdf_gstate *gs = pr->gstate + pr->gtop;
	pdf_material *mat;
	mat = what == PDF_FILL ? &gs->fill : &gs->stroke;
	if (mat->kind == PDF_MAT_PATTERN)
	{
		pdf_drop_pattern(ctx, mat->pattern);
		mat->pattern = NULL;
		mat->kind = PDF_MAT_COLOR;
	}
}