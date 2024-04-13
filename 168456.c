fz_cmm_avoid_white_fix_flag(fz_context *ctx)
{
	if (ctx && ctx->colorspace && ctx->colorspace->cmm && ctx->cmm_instance)
		return ctx->colorspace->cmm->avoid_white_fix_flag;
	return 0;
}