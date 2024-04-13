static void pdf_run_J(fz_context *ctx, pdf_processor *proc, int linecap)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pdf_flush_text(ctx, pr);

	pr->dev->flags &= ~(FZ_DEVFLAG_STARTCAP_UNDEFINED | FZ_DEVFLAG_DASHCAP_UNDEFINED | FZ_DEVFLAG_ENDCAP_UNDEFINED);
	gstate->stroke_state = fz_unshare_stroke_state(ctx, gstate->stroke_state);
	gstate->stroke_state->start_cap = linecap;
	gstate->stroke_state->dash_cap = linecap;
	gstate->stroke_state->end_cap = linecap;
}