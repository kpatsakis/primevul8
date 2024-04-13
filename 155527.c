static void pdf_run_rg(fz_context *ctx, pdf_processor *proc, float r, float g, float b)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	float color[3] = {r, g, b};
	pr->dev->flags &= ~FZ_DEVFLAG_FILLCOLOR_UNDEFINED;
	pdf_set_colorspace(ctx, pr, PDF_FILL, fz_device_rgb(ctx));
	pdf_set_color(ctx, pr, PDF_FILL, color);
}