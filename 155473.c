static void pdf_run_Tr(fz_context *ctx, pdf_processor *proc, int render)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	gstate->text.render = render;
}