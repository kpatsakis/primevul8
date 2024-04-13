fz_new_indexed_colorspace(fz_context *ctx, fz_colorspace *base, int high, unsigned char *lookup)
{
	fz_colorspace *cs = NULL;
	struct indexed *idx;

	idx = fz_malloc_struct(ctx, struct indexed);
	idx->lookup = lookup;
	idx->base = fz_keep_colorspace(ctx, base);
	idx->high = high;

	fz_try(ctx)
		cs = fz_new_colorspace(ctx, "Indexed", FZ_COLORSPACE_INDEXED, 0, 1, fz_colorspace_is_icc(ctx, fz_device_rgb(ctx)) ? indexed_to_alt : indexed_to_rgb, NULL, base_indexed, clamp_indexed, free_indexed, idx, sizeof(*idx) + (base->n * (idx->high + 1)) + base->size);
	fz_catch(ctx)
	{
		fz_free(ctx, idx);
		fz_rethrow(ctx);
	}
	return cs;
}