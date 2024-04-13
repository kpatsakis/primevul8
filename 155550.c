static void pdf_run_gs_BM(fz_context *ctx, pdf_processor *proc, const char *blendmode)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pdf_flush_text(ctx, pr);
	gstate->blendmode = fz_lookup_blendmode(blendmode);
}