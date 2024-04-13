pdf_flush_text(fz_context *ctx, pdf_run_processor *pr)
{
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	fz_text *text;
	int dofill;
	int dostroke;
	int doclip;
	int doinvisible;
	softmask_save softmask = { NULL };
	int knockout_group = 0;

	text = pdf_tos_get_text(ctx, &pr->tos);
	if (!text)
		return gstate;

	dofill = dostroke = doclip = doinvisible = 0;
	switch (pr->tos.text_mode)
	{
	case 0: dofill = 1; break;
	case 1: dostroke = 1; break;
	case 2: dofill = dostroke = 1; break;
	case 3: doinvisible = 1; break;
	case 4: dofill = doclip = 1; break;
	case 5: dostroke = doclip = 1; break;
	case 6: dofill = dostroke = doclip = 1; break;
	case 7: doclip = 1; break;
	}

	if (pr->super.hidden)
		dostroke = dofill = 0;

	fz_try(ctx)
	{
		fz_rect tb = pr->tos.text_bbox;

		fz_transform_rect(&tb, &gstate->ctm);
		if (dostroke)
			fz_adjust_rect_for_stroke(ctx, &tb, gstate->stroke_state, &gstate->ctm);

		/* Don't bother sending a text group with nothing in it */
		if (!text->head)
			break;

		if (dofill || dostroke)
			gstate = pdf_begin_group(ctx, pr, &tb, &softmask);

		if (dofill && dostroke)
		{
			/* We may need to push a knockout group */
			if (gstate->stroke.alpha == 0)
			{
				/* No need for group, as stroke won't do anything */
			}
			else if (gstate->stroke.alpha == 1.0f && gstate->blendmode == FZ_BLEND_NORMAL)
			{
				/* No need for group, as stroke won't show up */
			}
			else
			{
				knockout_group = 1;
				fz_begin_group(ctx, pr->dev, &tb, NULL, 0, 1, FZ_BLEND_NORMAL, 1);
			}
		}

		if (doinvisible)
			fz_ignore_text(ctx, pr->dev, text, &gstate->ctm);

		if (dofill)
		{
			switch (gstate->fill.kind)
			{
			case PDF_MAT_NONE:
				break;
			case PDF_MAT_COLOR:
				fz_fill_text(ctx, pr->dev, text, &gstate->ctm,
					gstate->fill.colorspace, gstate->fill.v, gstate->fill.alpha, &gstate->fill.color_params);
				break;
			case PDF_MAT_PATTERN:
				if (gstate->fill.pattern)
				{
					fz_clip_text(ctx, pr->dev, text, &gstate->ctm, &tb);
					pdf_show_pattern(ctx, pr, gstate->fill.pattern, &pr->gstate[gstate->fill.gstate_num], &tb, PDF_FILL);
					fz_pop_clip(ctx, pr->dev);
				}
				break;
			case PDF_MAT_SHADE:
				if (gstate->fill.shade)
				{
					fz_clip_text(ctx, pr->dev, text, &gstate->ctm, &tb);
					/* Page 2 of patterns.pdf shows that fz_fill_shade should NOT be called with gstate->ctm */
					fz_fill_shade(ctx, pr->dev, gstate->fill.shade, &pr->gstate[gstate->fill.gstate_num].ctm, gstate->fill.alpha, &gstate->fill.color_params);
					fz_pop_clip(ctx, pr->dev);
				}
				break;
			}
		}

		if (dostroke)
		{
			switch (gstate->stroke.kind)
			{
			case PDF_MAT_NONE:
				break;
			case PDF_MAT_COLOR:
				fz_stroke_text(ctx, pr->dev, text, gstate->stroke_state, &gstate->ctm,
					gstate->stroke.colorspace, gstate->stroke.v, gstate->stroke.alpha, &gstate->stroke.color_params);
				break;
			case PDF_MAT_PATTERN:
				if (gstate->stroke.pattern)
				{
					fz_clip_stroke_text(ctx, pr->dev, text, gstate->stroke_state, &gstate->ctm, &tb);
					pdf_show_pattern(ctx, pr, gstate->stroke.pattern, &pr->gstate[gstate->stroke.gstate_num], &tb, PDF_STROKE);
					fz_pop_clip(ctx, pr->dev);
				}
				break;
			case PDF_MAT_SHADE:
				if (gstate->stroke.shade)
				{
					fz_clip_stroke_text(ctx, pr->dev, text, gstate->stroke_state, &gstate->ctm, &tb);
					fz_fill_shade(ctx, pr->dev, gstate->stroke.shade, &pr->gstate[gstate->stroke.gstate_num].ctm, gstate->stroke.alpha, &gstate->stroke.color_params);
					fz_pop_clip(ctx, pr->dev);
				}
				break;
			}
		}

		if (knockout_group)
			fz_end_group(ctx, pr->dev);

		if (dofill || dostroke)
			pdf_end_group(ctx, pr, &softmask);

		if (doclip)
		{
			gstate->clip_depth++;
			fz_clip_text(ctx, pr->dev, text, &gstate->ctm, &tb);
		}
	}
	fz_always(ctx)
	{
		fz_drop_text(ctx, text);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}

	return pr->gstate + pr->gtop;
}