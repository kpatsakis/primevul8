static void pdf_run_dquote(fz_context *ctx, pdf_processor *proc, float aw, float ac, char *string, int string_len)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	gstate->text.word_space = aw;
	gstate->text.char_space = ac;
	pdf_tos_newline(&pr->tos, gstate->text.leading);
	pdf_show_string(ctx, pr, (unsigned char*)string, string_len);
}