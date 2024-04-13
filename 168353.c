fz_cmm_transform_pixmap(fz_context *ctx, fz_icclink *link, fz_pixmap *dst, fz_pixmap *src)
{
	if (ctx && ctx->colorspace && ctx->colorspace->cmm && ctx->cmm_instance)
		ctx->colorspace->cmm->transform_pixmap(ctx->cmm_instance, link, dst, src);
}