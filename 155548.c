static void pdf_run_v(fz_context *ctx, pdf_processor *proc, float x2, float y2, float x3, float y3)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	fz_curvetov(ctx, pr->path, x2, y2, x3, y3);
}