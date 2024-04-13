static void pdf_run_k(fz_context *ctx, pdf_processor *proc, float c, float m, float y, float k)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	float color[4] = {c, m, y, k};
	pr->dev->flags &= ~FZ_DEVFLAG_FILLCOLOR_UNDEFINED;
	pdf_set_colorspace(ctx, pr, PDF_FILL, fz_device_cmyk(ctx));
	pdf_set_color(ctx, pr, PDF_FILL, color);
}