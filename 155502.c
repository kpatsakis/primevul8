static void pdf_run_EMC(fz_context *ctx, pdf_processor *proc)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;

	fz_end_layer(ctx, pr->dev);
}