void fz_cmm_drop_instance(fz_context *ctx)
{
	if (ctx && ctx->colorspace && ctx->colorspace->cmm && ctx->cmm_instance)
		ctx->colorspace->cmm->drop_instance(ctx->cmm_instance);
}