static void pdf_run_SC_color(fz_context *ctx, pdf_processor *proc, int n, float *color)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pr->dev->flags &= ~FZ_DEVFLAG_STROKECOLOR_UNDEFINED;
	pdf_set_color(ctx, pr, PDF_STROKE, color);
}