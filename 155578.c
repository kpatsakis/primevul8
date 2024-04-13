static void pdf_run_Do_form(fz_context *ctx, pdf_processor *proc, const char *name, pdf_xobject *xobj, pdf_obj *page_resources)
{
	pdf_run_xobject(ctx, (pdf_run_processor*)proc, xobj, page_resources, &fz_identity, 0);
}