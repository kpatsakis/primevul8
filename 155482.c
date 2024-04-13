static void pdf_run_BT(fz_context *ctx, pdf_processor *proc)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pr->tos.tm = fz_identity;
	pr->tos.tlm = fz_identity;
}