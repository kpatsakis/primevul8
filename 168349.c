void fz_set_cmm_engine(fz_context *ctx, const fz_cmm_engine *engine)
{
	fz_colorspace_context *cct;

	if (!ctx)
		return;
	cct = ctx->colorspace;
	if (!cct)
		return;

#ifdef NO_ICC
	if (engine)
		fz_throw(ctx, FZ_ERROR_GENERIC, "ICC workflow not supported in NO_ICC build");
#else
	if (cct->cmm == engine)
		return;

	fz_drop_cmm_context(ctx);
	fz_drop_colorspace(ctx, cct->gray);
	fz_drop_colorspace(ctx, cct->rgb);
	fz_drop_colorspace(ctx, cct->bgr);
	fz_drop_colorspace(ctx, cct->cmyk);
	fz_drop_colorspace(ctx, cct->lab);
	cct->gray = NULL;
	cct->rgb = NULL;
	cct->bgr = NULL;
	cct->cmyk = NULL;
	cct->lab = NULL;
	cct->cmm = engine;
	fz_new_cmm_context(ctx);
	if (engine)
	{
		cct->gray = fz_new_icc_colorspace(ctx, FZ_ICC_PROFILE_GRAY, 1, NULL);
		cct->rgb = fz_new_icc_colorspace(ctx, FZ_ICC_PROFILE_RGB, 3, NULL);
		cct->bgr = fz_new_icc_colorspace(ctx, FZ_ICC_PROFILE_BGR, 3, NULL);
		cct->cmyk = fz_new_icc_colorspace(ctx, FZ_ICC_PROFILE_CMYK, 4, NULL);
		cct->lab = fz_new_icc_colorspace(ctx, FZ_ICC_PROFILE_LAB, 3, NULL);
	}
	else
		set_no_icc(cct);
#endif
}