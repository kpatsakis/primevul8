static void pdf_run_Tz(fz_context *ctx, pdf_processor *proc, float scale)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	gstate->text.scale = scale / 100;
}