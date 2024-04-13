static void pdf_run_BDC(fz_context *ctx, pdf_processor *proc, const char *tag, pdf_obj *raw, pdf_obj *cooked)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	const char *str;

	if (!tag || strcmp(tag, "OC"))
		return;

	str = pdf_to_str_buf(ctx, pdf_dict_get(ctx, cooked, PDF_NAME_Name));
	if (str == NULL)
		str = "UnnamedLayer";

	fz_begin_layer(ctx, pr->dev, str);
}