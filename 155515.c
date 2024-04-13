pdf_show_char(fz_context *ctx, pdf_run_processor *pr, int cid)
{
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	pdf_font_desc *fontdesc = gstate->text.font;
	fz_matrix trm;
	int gid;
	int ucsbuf[8];
	int ucslen;
	int i;
	int render_direct;

	gid = pdf_tos_make_trm(ctx, &pr->tos, &gstate->text, fontdesc, cid, &trm);

	/* If we are a type3 font within a type 3 font, or are otherwise
	 * uncachable, then render direct. */
	render_direct = (!fz_font_ft_face(ctx, fontdesc->font) && pr->nested_depth > 0) || !fz_glyph_cacheable(ctx, fontdesc->font, gid);

	/* flush buffered text if rendermode has changed */
	if (!pr->tos.text || gstate->text.render != pr->tos.text_mode || render_direct)
	{
		gstate = pdf_flush_text(ctx, pr);
		pdf_tos_reset(ctx, &pr->tos, gstate->text.render);
	}

	if (render_direct)
	{
		/* Render the glyph stream direct here (only happens for
		 * type3 glyphs that seem to inherit current graphics
		 * attributes, or type 3 glyphs within type3 glyphs). */
		fz_matrix composed;
		fz_concat(&composed, &trm, &gstate->ctm);
		fz_render_t3_glyph_direct(ctx, pr->dev, fontdesc->font, gid, &composed, gstate, pr->nested_depth, pr->default_cs);
		/* Render text invisibly so that it can still be extracted. */
		pr->tos.text_mode = 3;
	}

	ucslen = 0;
	if (fontdesc->to_unicode)
		ucslen = pdf_lookup_cmap_full(fontdesc->to_unicode, cid, ucsbuf);
	if (ucslen == 0 && (size_t)cid < fontdesc->cid_to_ucs_len)
	{
		ucsbuf[0] = fontdesc->cid_to_ucs[cid];
		ucslen = 1;
	}
	if (ucslen == 0 || (ucslen == 1 && ucsbuf[0] == 0))
	{
		ucsbuf[0] = FZ_REPLACEMENT_CHARACTER;
		ucslen = 1;
	}

	/* add glyph to textobject */
	fz_show_glyph(ctx, pr->tos.text, fontdesc->font, &trm, gid, ucsbuf[0], fontdesc->wmode, 0, FZ_BIDI_NEUTRAL, FZ_LANG_UNSET);

	/* add filler glyphs for one-to-many unicode mapping */
	for (i = 1; i < ucslen; i++)
		fz_show_glyph(ctx, pr->tos.text, fontdesc->font, &trm, -1, ucsbuf[i], fontdesc->wmode, 0, FZ_BIDI_NEUTRAL, FZ_LANG_UNSET);

	pdf_tos_move_after_char(ctx, &pr->tos);
}