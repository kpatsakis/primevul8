pdf_show_shade(fz_context *ctx, pdf_run_processor *pr, fz_shade *shd)
{
	pdf_gstate *gstate = pr->gstate + pr->gtop;
	fz_rect bbox;
	softmask_save softmask = { NULL };

	if (pr->super.hidden)
		return;

	fz_bound_shade(ctx, shd, &gstate->ctm, &bbox);

	gstate = pdf_begin_group(ctx, pr, &bbox, &softmask);

	/* FIXME: The gstate->ctm in the next line may be wrong; maybe
	 * it should be the parent gstates ctm? */
	fz_fill_shade(ctx, pr->dev, shd, &gstate->ctm, gstate->fill.alpha, &gstate->fill.color_params);

	pdf_end_group(ctx, pr, &softmask);
}