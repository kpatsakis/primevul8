fz_keep_colorspace_context(fz_context *ctx)
{
	if (!ctx)
		return NULL;
	return fz_keep_imp(ctx, ctx->colorspace, &ctx->colorspace->ctx_refs);
}