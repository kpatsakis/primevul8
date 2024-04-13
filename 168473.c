fz_keep_default_colorspaces(fz_context *ctx, fz_default_colorspaces *default_cs)
{
	return fz_keep_imp(ctx, default_cs, &default_cs->refs);
}