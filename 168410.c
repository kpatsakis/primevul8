const fz_cmm_engine *fz_get_cmm_engine(fz_context *ctx)
{
	return ctx->colorspace ? ctx->colorspace->cmm : NULL;
}