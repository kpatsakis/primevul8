static void pdf_run_b(fz_context *ctx, pdf_processor *proc)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_show_path(ctx, pr, 1, 1, 1, 0);
}