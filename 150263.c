static struct ucma_context *ucma_get_ctx_dev(struct ucma_file *file, int id)
{
	struct ucma_context *ctx = ucma_get_ctx(file, id);

	if (IS_ERR(ctx))
		return ctx;
	if (!ctx->cm_id->device) {
		ucma_put_ctx(ctx);
		return ERR_PTR(-EINVAL);
	}
	return ctx;
}