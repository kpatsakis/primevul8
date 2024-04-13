static void pdf_run_l(fz_context *ctx, pdf_processor *proc, float x, float y)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	fz_lineto(ctx, pr->path, x, y);
}