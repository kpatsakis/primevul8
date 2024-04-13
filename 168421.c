fz_cmm_instance *fz_cmm_new_instance(fz_context *ctx)
{
	if (ctx && ctx->colorspace && ctx->colorspace->cmm)
		return ctx->colorspace->cmm->new_instance(ctx);
	return NULL;
}