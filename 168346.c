fz_keep_colorspace_store_key(fz_context *ctx, fz_colorspace *cs)
{
	return fz_keep_key_storable_key(ctx, &cs->key_storable);
}