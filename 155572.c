static void pdf_run_W(fz_context *ctx, pdf_processor *proc)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pr->clip = 1;
	pr->clip_even_odd = 0;
}