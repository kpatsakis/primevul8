static void pdf_run_q(fz_context *ctx, pdf_processor *proc)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gsave(ctx, pr);
}