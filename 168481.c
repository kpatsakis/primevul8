fz_indexed_colorspace_palette(fz_context *ctx, fz_colorspace *cs, int *high)
{
	struct indexed *idx = cs->data;
	if (!fz_colorspace_is_indexed(ctx, cs))
		fz_throw(ctx, FZ_ERROR_GENERIC, "colorspace not indexed");
	*high = idx->high;
	return idx->lookup;
}