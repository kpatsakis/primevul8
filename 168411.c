void fz_cmm_fin_profile(fz_context *ctx, fz_iccprofile *profile)
{
	if (ctx && ctx->colorspace && ctx->colorspace->cmm && ctx->cmm_instance)
		if (profile && profile->cmm_handle != NULL)
			ctx->colorspace->cmm->fin_profile(ctx->cmm_instance, profile);
}