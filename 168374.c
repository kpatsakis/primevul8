fz_drop_colorspace_store_key(fz_context *ctx, fz_colorspace *cs)
{
	fz_drop_key_storable_key(ctx, &cs->key_storable);
}