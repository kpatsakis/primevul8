static void pdf_run_Ts(fz_context *ctx, pdf_processor *proc, float rise)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	gstate->text.rise = rise;
}