pdf_show_space(fz_context *ctx, pdf_run_processor *pr, float tadj)
{
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	pdf_font_desc *fontdesc = gstate->text.font;

	if (fontdesc->wmode == 0)
		fz_pre_translate(&pr->tos.tm, tadj * gstate->text.scale, 0);
	else
		fz_pre_translate(&pr->tos.tm, 0, tadj);
}