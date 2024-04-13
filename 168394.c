fz_new_cmm_context(fz_context *ctx)
{
	ctx->cmm_instance = fz_cmm_new_instance(ctx);
}