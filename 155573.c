static void pdf_run_re(fz_context *ctx, pdf_processor *proc, float x, float y, float w, float h)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	fz_rectto(ctx, pr->path, x, y, x+w, y+h);
}