static void pdf_run_BMC(fz_context *ctx, pdf_processor *proc, const char *tag)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;

	if (!tag)
		tag = "UnnamedLayer";

	fz_begin_layer(ctx, pr->dev, tag);
}