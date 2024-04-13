pdf_show_image(fz_context *ctx, pdf_run_processor *pr, fz_image *image)
{
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	fz_matrix image_ctm;
	fz_rect bbox;

	if (pr->super.hidden)
		return;

	/* PDF has images bottom-up, so flip them right side up here */
	image_ctm = gstate->ctm;
	fz_pre_scale(fz_pre_translate(&image_ctm, 0, 1), 1, -1);

	bbox = fz_unit_rect;
	fz_transform_rect(&bbox, &image_ctm);

	if (image->mask && gstate->blendmode)
	{
		/* apply blend group even though we skip the soft mask */
		fz_begin_group(ctx, pr->dev, &bbox, NULL, 0, 0, gstate->blendmode, 1);

		fz_try(ctx)
			fz_clip_image_mask(ctx, pr->dev, image->mask, &image_ctm, &bbox);
		fz_catch(ctx)
		{
			fz_end_group(ctx, pr->dev);
			fz_rethrow(ctx);
		}

		fz_try(ctx)
			pdf_show_image_imp(ctx, pr, image, &image_ctm, &bbox);
		fz_always(ctx)
		{
			fz_pop_clip(ctx, pr->dev);
			fz_end_group(ctx, pr->dev);
		}
		fz_catch(ctx)
			fz_rethrow(ctx);
	}
	else if (image->mask)
	{
		fz_clip_image_mask(ctx, pr->dev, image->mask, &image_ctm, &bbox);
		fz_try(ctx)
			pdf_show_image_imp(ctx, pr, image, &image_ctm, &bbox);
		fz_always(ctx)
			fz_pop_clip(ctx, pr->dev);
		fz_catch(ctx)
			fz_rethrow(ctx);
	}
	else
	{
		softmask_save softmask = { NULL };

		gstate = pdf_begin_group(ctx, pr, &bbox, &softmask);

		fz_try(ctx)
			pdf_show_image_imp(ctx, pr, image, &image_ctm, &bbox);
		fz_always(ctx)
			pdf_end_group(ctx, pr, &softmask);
		fz_catch(ctx)
			fz_rethrow(ctx);
	}
}