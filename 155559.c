static void pdf_run_END(fz_context *ctx, pdf_processor *proc)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_flush_text(ctx, pr);
}