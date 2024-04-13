static void pdf_run_w(fz_context *ctx, pdf_processor *proc, float linewidth)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pdf_flush_text(ctx, pr);

	pr->dev->flags &= ~FZ_DEVFLAG_LINEWIDTH_UNDEFINED;
	gstate->stroke_state = fz_unshare_stroke_state(ctx, gstate->stroke_state);
	gstate->stroke_state->linewidth = linewidth;
}