fz_drop_cmm_context(fz_context *ctx)
{
	fz_cmm_drop_instance(ctx);
	ctx->cmm_instance = NULL;
}