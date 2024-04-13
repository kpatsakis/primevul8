static void pdf_run_d0(fz_context *ctx, pdf_processor *proc, float wx, float wy)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	if (pr->nested_depth > 1)
		return;
	pr->dev->flags |= FZ_DEVFLAG_COLOR;
}