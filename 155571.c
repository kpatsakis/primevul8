static void pdf_run_squote(fz_context *ctx, pdf_processor *proc, char *string, int string_len)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	pdf_tos_newline(&pr->tos, gstate->text.leading);
	pdf_show_string(ctx, pr, (unsigned char*)string, string_len);
}