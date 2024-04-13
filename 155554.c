static void pdf_run_ri(fz_context *ctx, pdf_processor *proc, const char *intent)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pdf_flush_text(ctx, pr);
	gstate->fill.color_params.ri = fz_lookup_rendering_intent(intent);
	gstate->stroke.color_params.ri = gstate->fill.color_params.ri;
}