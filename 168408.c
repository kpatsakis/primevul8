fz_cmm_transform_color(fz_context *ctx, fz_icclink *link, unsigned short *dst, const unsigned short *src)
{
	if (ctx && ctx->colorspace && ctx->colorspace->cmm && ctx->cmm_instance)
		ctx->colorspace->cmm->transform_color(ctx->cmm_instance, link, dst, src);
}