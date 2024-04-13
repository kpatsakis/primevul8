fz_keep_colorspace(fz_context *ctx, fz_colorspace *cs)
{
	return fz_keep_key_storable(ctx, &cs->key_storable);
}