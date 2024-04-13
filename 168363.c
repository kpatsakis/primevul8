static void fz_cached_color_convert(fz_context *ctx, fz_color_converter *cc_, float *ds, const float *ss)
{
	fz_cached_color_converter *cc = cc_->opaque;
	void *val = fz_hash_find(ctx, cc->hash, ss);
	int n = cc->base.ds->n * sizeof(float);
	fz_color_converter *base_cc = &cc->base;

	if (val)
	{
		memcpy(ds, val, n);
		return;
	}

	base_cc->convert(ctx, base_cc, ds, ss);
	val = fz_malloc(ctx, n);
	memcpy(val, ds, n);
	fz_try(ctx)
	{
		fz_hash_insert(ctx, cc->hash, ss, val);
	}
	fz_catch(ctx)
	{
		fz_free(ctx, val);
	}
}