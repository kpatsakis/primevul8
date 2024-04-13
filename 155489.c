pdf_show_image_imp(fz_context *ctx, pdf_run_processor *pr, fz_image *image, fz_matrix *image_ctm, fz_rect *bbox)
{
	pdf_gstate *gstate = pr->gstate + pr->gtop;

	if (image->colorspace)
	{
		fz_fill_image(ctx, pr->dev, image, image_ctm, gstate->fill.alpha, &gstate->fill.color_params);
		return;
	}

	if (gstate->fill.kind == PDF_MAT_COLOR)
		fz_fill_image_mask(ctx, pr->dev, image, image_ctm,
				gstate->fill.colorspace, gstate->fill.v, gstate->fill.alpha, &gstate->fill.color_params);
	else if (gstate->fill.kind == PDF_MAT_PATTERN && gstate->fill.pattern)
	{
		fz_clip_image_mask(ctx, pr->dev, image, image_ctm, bbox);
		fz_try(ctx)
			pdf_show_pattern(ctx, pr, gstate->fill.pattern, &pr->gstate[gstate->fill.gstate_num], bbox, PDF_FILL);
		fz_always(ctx)
			fz_pop_clip(ctx, pr->dev);
		fz_catch(ctx)
			fz_rethrow(ctx);
	}
	else if (gstate->fill.kind == PDF_MAT_SHADE && gstate->fill.shade)
	{
		fz_clip_image_mask(ctx, pr->dev, image, image_ctm, bbox);
		fz_try(ctx)
			fz_fill_shade(ctx, pr->dev, gstate->fill.shade, &pr->gstate[gstate->fill.gstate_num].ctm, gstate->fill.alpha, &gstate->fill.color_params);
		fz_always(ctx)
			fz_pop_clip(ctx, pr->dev);
		fz_catch(ctx)
			fz_rethrow(ctx);
	}
}