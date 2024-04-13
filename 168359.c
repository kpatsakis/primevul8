void fz_cmm_init_profile(fz_context *ctx, fz_iccprofile *profile)
{
	if (ctx && ctx->colorspace && ctx->colorspace->cmm && ctx->cmm_instance)
		ctx->colorspace->cmm->init_profile(ctx->cmm_instance, profile);
}