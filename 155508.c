end_softmask(fz_context *ctx, pdf_run_processor *pr, softmask_save *save)
{
	pdf_gstate *gstate = pr->gstate + pr->gtop;

	if (save->softmask == NULL)
		return;

	gstate->softmask = save->softmask;
	gstate->softmask_resources = save->page_resources;
	gstate->softmask_ctm = save->ctm;
	fz_pop_clip(ctx, pr->dev);
}