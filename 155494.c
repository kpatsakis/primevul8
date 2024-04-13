static void pdf_run_sc_pattern(fz_context *ctx, pdf_processor *proc, const char *name, pdf_pattern *pat, int n, float *color)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pr->dev->flags &= ~FZ_DEVFLAG_FILLCOLOR_UNDEFINED;
	pdf_set_pattern(ctx, pr, PDF_FILL, pat, color);
}