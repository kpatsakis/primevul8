static void pdf_run_SC_shade(fz_context *ctx, pdf_processor *proc, const char *name, fz_shade *shade)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pr->dev->flags &= ~FZ_DEVFLAG_STROKECOLOR_UNDEFINED;
	pdf_set_shade(ctx, pr, PDF_STROKE, shade);
}