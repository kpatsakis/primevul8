static void pdf_run_gs_op(fz_context *ctx, pdf_processor *proc, int b)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pdf_flush_text(ctx, pr);
	gstate->fill.color_params.op = b;
}