free_indexed(fz_context *ctx, fz_colorspace *cs)
{
	struct indexed *idx = cs->data;
	fz_drop_colorspace(ctx, idx->base);
	fz_free(ctx, idx->lookup);
	fz_free(ctx, idx);
}