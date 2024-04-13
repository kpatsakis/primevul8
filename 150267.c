static void ucma_finish_ctx(struct ucma_context *ctx)
{
	lockdep_assert_held(&ctx->file->mut);
	list_add_tail(&ctx->list, &ctx->file->ctx_list);
	xa_store(&ctx_table, ctx->id, ctx, GFP_KERNEL);
}