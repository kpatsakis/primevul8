static struct ucma_context *ucma_alloc_ctx(struct ucma_file *file)
{
	struct ucma_context *ctx;

	ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return NULL;

	INIT_WORK(&ctx->close_work, ucma_close_id);
	refcount_set(&ctx->ref, 1);
	init_completion(&ctx->comp);
	/* So list_del() will work if we don't do ucma_finish_ctx() */
	INIT_LIST_HEAD(&ctx->list);
	ctx->file = file;
	mutex_init(&ctx->mutex);

	if (xa_alloc(&ctx_table, &ctx->id, NULL, xa_limit_32b, GFP_KERNEL)) {
		kfree(ctx);
		return NULL;
	}
	return ctx;
}