static void pdf_run_gs_OPM(fz_context *ctx, pdf_processor *proc, int i)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pdf_flush_text(ctx, pr);
	gstate->stroke.color_params.opm = i;
	gstate->fill.color_params.opm = i;
}