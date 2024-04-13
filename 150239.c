static void ucma_put_ctx(struct ucma_context *ctx)
{
	if (refcount_dec_and_test(&ctx->ref))
		complete(&ctx->comp);
}