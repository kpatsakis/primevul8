static void pdf_run_Tstar(fz_context *ctx, pdf_processor *proc)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	pdf_tos_newline(&pr->tos, gstate->text.leading);
}