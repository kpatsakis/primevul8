pdf_show_path(fz_context *ctx, pdf_run_processor *pr, int doclose, int dofill, int dostroke, int even_odd)
{
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	fz_path *path;
	fz_rect bbox;
	softmask_save softmask = { NULL };
	int knockout_group = 0;

	if (dostroke) {
		if (pr->dev->flags & (FZ_DEVFLAG_STROKECOLOR_UNDEFINED | FZ_DEVFLAG_LINEJOIN_UNDEFINED | FZ_DEVFLAG_LINEWIDTH_UNDEFINED))
			pr->dev->flags |= FZ_DEVFLAG_UNCACHEABLE;
		else if (gstate->stroke_state->dash_len != 0 && pr->dev->flags & (FZ_DEVFLAG_STARTCAP_UNDEFINED | FZ_DEVFLAG_DASHCAP_UNDEFINED | FZ_DEVFLAG_ENDCAP_UNDEFINED))
			pr->dev->flags |= FZ_DEVFLAG_UNCACHEABLE;
		else if (gstate->stroke_state->linejoin == FZ_LINEJOIN_MITER && (pr->dev->flags & FZ_DEVFLAG_MITERLIMIT_UNDEFINED))
			pr->dev->flags |= FZ_DEVFLAG_UNCACHEABLE;
	}
	if (dofill) {
		if (pr->dev->flags & FZ_DEVFLAG_FILLCOLOR_UNDEFINED)
			pr->dev->flags |= FZ_DEVFLAG_UNCACHEABLE;
	}

	path = pr->path;
	pr->path = fz_new_path(ctx);

	fz_try(ctx)
	{
		if (doclose)
			fz_closepath(ctx, path);

		fz_bound_path(ctx, path, (dostroke ? gstate->stroke_state : NULL), &gstate->ctm, &bbox);

		if (pr->clip)
		{
			gstate->clip_depth++;
			fz_clip_path(ctx, pr->dev, path, pr->clip_even_odd, &gstate->ctm, &bbox);
			pr->clip = 0;
		}

		if (pr->super.hidden)
			dostroke = dofill = 0;

		if (dofill || dostroke)
			gstate = pdf_begin_group(ctx, pr, &bbox, &softmask);

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
				fz_begin_group(ctx, pr->dev, &bbox, NULL, 0, 1, FZ_BLEND_NORMAL, 1);
			}
		}

		if (dofill)
		{
			switch (gstate->fill.kind)
			{
			case PDF_MAT_NONE:
				break;
			case PDF_MAT_COLOR:
				fz_fill_path(ctx, pr->dev, path, even_odd, &gstate->ctm,
					gstate->fill.colorspace, gstate->fill.v, gstate->fill.alpha, &gstate->fill.color_params);
				break;
			case PDF_MAT_PATTERN:
				if (gstate->fill.pattern)
				{
					fz_clip_path(ctx, pr->dev, path, even_odd, &gstate->ctm, &bbox);
					pdf_show_pattern(ctx, pr, gstate->fill.pattern, &pr->gstate[gstate->fill.gstate_num], &bbox, PDF_FILL);
					fz_pop_clip(ctx, pr->dev);
				}
				break;
			case PDF_MAT_SHADE:
				if (gstate->fill.shade)
				{
					fz_clip_path(ctx, pr->dev, path, even_odd, &gstate->ctm, &bbox);
					/* The cluster and page 2 of patterns.pdf shows that fz_fill_shade should NOT be called with gstate->ctm. */
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
				fz_stroke_path(ctx, pr->dev, path, gstate->stroke_state, &gstate->ctm,
					gstate->stroke.colorspace, gstate->stroke.v, gstate->stroke.alpha, &gstate->stroke.color_params);
				break;
			case PDF_MAT_PATTERN:
				if (gstate->stroke.pattern)
				{
					fz_clip_stroke_path(ctx, pr->dev, path, gstate->stroke_state, &gstate->ctm, &bbox);
					pdf_show_pattern(ctx, pr, gstate->stroke.pattern, &pr->gstate[gstate->stroke.gstate_num], &bbox, PDF_STROKE);
					fz_pop_clip(ctx, pr->dev);
				}
				break;
			case PDF_MAT_SHADE:
				if (gstate->stroke.shade)
				{
					fz_clip_stroke_path(ctx, pr->dev, path, gstate->stroke_state, &gstate->ctm, &bbox);
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
	}
	fz_always(ctx)
	{
		fz_drop_path(ctx, path);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}