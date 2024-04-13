void fz_new_colorspace_context(fz_context *ctx)
{
	ctx->colorspace = fz_malloc_struct(ctx, fz_colorspace_context);
	ctx->colorspace->ctx_refs = 1;
	set_no_icc(ctx->colorspace);
#ifdef NO_ICC
	fz_set_cmm_engine(ctx, NULL);
#else
	fz_set_cmm_engine(ctx, &fz_cmm_engine_lcms);
#endif
}