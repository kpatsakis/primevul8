pdf_show_string(fz_context *ctx, pdf_run_processor *pr, unsigned char *buf, int len)
{
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	pdf_font_desc *fontdesc = gstate->text.font;

	if (!fontdesc)
	{
		fz_warn(ctx, "cannot draw text since font and size not set");
		return;
	}

	show_string(ctx, pr, buf, len);
}