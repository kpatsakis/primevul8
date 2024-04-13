static void pdf_run_Q(fz_context *ctx, pdf_processor *proc)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_grestore(ctx, pr);
}