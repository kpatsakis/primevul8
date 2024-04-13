pdf_grestore(fz_context *ctx, pdf_run_processor *pr)
{
	pdf_gstate *gs = pr->gstate + pr->gtop;
	int clip_depth = gs->clip_depth;

	if (pr->gtop <= pr->gbot)
	{
		fz_warn(ctx, "gstate underflow in content stream");
		return;
	}

	pdf_drop_gstate(ctx, gs);
	pr->gtop --;

	gs = pr->gstate + pr->gtop;
	while (clip_depth > gs->clip_depth)
	{
		fz_try(ctx)
		{
			fz_pop_clip(ctx, pr->dev);
		}
		fz_catch(ctx)
		{
			/* Silently swallow the problem - restores must
			 * never throw! */
		}
		clip_depth--;
	}
}