static void pdf_run_g(fz_context *ctx, pdf_processor *proc, float g)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pr->dev->flags &= ~FZ_DEVFLAG_FILLCOLOR_UNDEFINED;
	pdf_set_colorspace(ctx, pr, PDF_FILL, fz_device_gray(ctx));
	pdf_set_color(ctx, pr, PDF_FILL, &g);
}