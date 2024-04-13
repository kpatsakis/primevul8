fz_cmm_fin_link(fz_context *ctx, fz_icclink *link)
{
	if (ctx && ctx->colorspace && ctx->colorspace->cmm && ctx->cmm_instance)
		ctx->colorspace->cmm->fin_link(ctx->cmm_instance, link);
}