static void pdf_run_gs_SMask(fz_context *ctx, pdf_processor *proc, pdf_xobject *smask, pdf_obj *page_resources, float *bc, int luminosity)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = pdf_flush_text(ctx, pr);
	int i;

	if (gstate->softmask)
	{
		pdf_drop_xobject(ctx, gstate->softmask);
		gstate->softmask = NULL;
		pdf_drop_obj(ctx, gstate->softmask_resources);
		gstate->softmask_resources = NULL;
	}

	if (smask)
	{
		fz_colorspace *cs = pdf_xobject_colorspace(ctx, smask);
		int cs_n = 1;
		if (cs)
			cs_n = fz_colorspace_n(ctx, cs);
		gstate->softmask_ctm = gstate->ctm;
		gstate->softmask = pdf_keep_xobject(ctx, smask);
		gstate->softmask_resources = pdf_keep_obj(ctx, page_resources);
		for (i = 0; i < cs_n; ++i)
			gstate->softmask_bc[i] = bc[i];
		gstate->luminosity = luminosity;
		fz_drop_colorspace(ctx, cs);
	}
}