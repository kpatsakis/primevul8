static void pdf_run_BI(fz_context *ctx, pdf_processor *proc, fz_image *image)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_show_image(ctx, pr, image);
}