static void pdf_run_CS(fz_context *ctx, pdf_processor *proc, const char *name, fz_colorspace *colorspace)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pr->dev->flags &= ~FZ_DEVFLAG_STROKECOLOR_UNDEFINED;
	if (!strcmp(name, "Pattern"))
		pdf_set_pattern(ctx, pr, PDF_STROKE, NULL, NULL);
	else
		pdf_set_colorspace(ctx, pr, PDF_STROKE, colorspace);
}