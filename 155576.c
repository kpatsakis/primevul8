pdf_begin_group(fz_context *ctx, pdf_run_processor *pr, const fz_rect *bbox, softmask_save *softmask)
{
	pdf_gstate *gstate = begin_softmask(ctx, pr, softmask);

	if (gstate->blendmode)
		fz_begin_group(ctx, pr->dev, bbox, NULL, 0, 0, gstate->blendmode, 1);

	return pr->gstate + pr->gtop;
}