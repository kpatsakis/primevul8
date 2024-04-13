static void pdf_run_y(fz_context *ctx, pdf_processor *proc, float x1, float y1, float x3, float y3)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	fz_curvetoy(ctx, pr->path, x1, y1, x3, y3);
}