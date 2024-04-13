fz_drop_colorspace(fz_context *ctx, fz_colorspace *cs)
{
	fz_drop_key_storable(ctx, &cs->key_storable);
}