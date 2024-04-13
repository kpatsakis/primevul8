static void pdf_run_TD(fz_context *ctx, pdf_processor *proc, float tx, float ty)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	gstate->text.leading = -ty;
	pdf_tos_translate(&pr->tos, tx, ty);
}