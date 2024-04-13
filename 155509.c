pdf_set_color(fz_context *ctx, pdf_run_processor *pr, int what, float *v)
{
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	pdf_material *mat;

	gstate = pdf_flush_text(ctx, pr);

	mat = what == PDF_FILL ? &gstate->fill : &gstate->stroke;

	switch (mat->kind)
	{
	case PDF_MAT_PATTERN:
	case PDF_MAT_COLOR:
		fz_clamp_color(ctx, mat->colorspace, v, mat->v);
		break;
	default:
		fz_warn(ctx, "color incompatible with material");
	}

	mat->gstate_num = pr->gparent;
}