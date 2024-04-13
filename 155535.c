pdf_set_shade(fz_context *ctx, pdf_run_processor *pr, int what, fz_shade *shade)
{
	pdf_gstate *gs;
	pdf_material *mat;

	gs = pdf_flush_text(ctx, pr);

	mat = what == PDF_FILL ? &gs->fill : &gs->stroke;

	fz_drop_shade(ctx, mat->shade);

	mat->kind = PDF_MAT_SHADE;
	mat->shade = fz_keep_shade(ctx, shade);

	mat->gstate_num = pr->gparent;
}