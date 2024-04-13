static void pdf_run_j(fz_context *ctx, pdf_processor *proc, int linejoin)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pdf_flush_text(ctx, pr);

	pr->dev->flags &= ~FZ_DEVFLAG_LINEJOIN_UNDEFINED;
	gstate->stroke_state = fz_unshare_stroke_state(ctx, gstate->stroke_state);
	gstate->stroke_state->linejoin = linejoin;
}