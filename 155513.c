pdf_end_group(fz_context *ctx, pdf_run_processor *pr, softmask_save *softmask)
{
	pdf_gstate *gstate = pr->gstate + pr->gtop;

	if (gstate->blendmode)
		fz_end_group(ctx, pr->dev);

	end_softmask(ctx, pr, softmask);
}