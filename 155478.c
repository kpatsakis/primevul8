static void pdf_run_Tc(fz_context *ctx, pdf_processor *proc, float charspace)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	gstate->text.char_space = charspace;
}