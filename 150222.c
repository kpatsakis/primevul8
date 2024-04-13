static struct ucma_context *ucma_get_ctx(struct ucma_file *file, int id)
{
	struct ucma_context *ctx;

	xa_lock(&ctx_table);
	ctx = _ucma_find_context(id, file);
	if (!IS_ERR(ctx))
		if (!refcount_inc_not_zero(&ctx->ref))
			ctx = ERR_PTR(-ENXIO);
	xa_unlock(&ctx_table);
	return ctx;
}