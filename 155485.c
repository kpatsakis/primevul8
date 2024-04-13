pdf_show_text(fz_context *ctx, pdf_run_processor *pr, pdf_obj *text)
{
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	pdf_font_desc *fontdesc = gstate->text.font;
	int i;

	if (!fontdesc)
	{
		fz_warn(ctx, "cannot draw text since font and size not set");
		return;
	}

	if (pdf_is_array(ctx, text))
	{
		int n = pdf_array_len(ctx, text);
		for (i = 0; i < n; i++)
		{
			pdf_obj *item = pdf_array_get(ctx, text, i);
			if (pdf_is_string(ctx, item))
				show_string(ctx, pr, (unsigned char *)pdf_to_str_buf(ctx, item), pdf_to_str_len(ctx, item));
			else
				pdf_show_space(ctx, pr, - pdf_to_real(ctx, item) * gstate->text.size * 0.001f);
		}
	}
	else if (pdf_is_string(ctx, text))
	{
		pdf_show_string(ctx, pr, (unsigned char *)pdf_to_str_buf(ctx, text), pdf_to_str_len(ctx, text));
	}
}