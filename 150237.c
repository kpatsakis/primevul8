static inline struct ucma_context *_ucma_find_context(int id,
						      struct ucma_file *file)
{
	struct ucma_context *ctx;

	ctx = xa_load(&ctx_table, id);
	if (!ctx)
		ctx = ERR_PTR(-ENOENT);
	else if (ctx->file != file)
		ctx = ERR_PTR(-EINVAL);
	return ctx;
}