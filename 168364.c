fz_drop_default_colorspaces(fz_context *ctx, fz_default_colorspaces *default_cs)
{
	if (fz_drop_imp(ctx, default_cs, &default_cs->refs))
	{
		fz_drop_colorspace(ctx, default_cs->gray);
		fz_drop_colorspace(ctx, default_cs->rgb);
		fz_drop_colorspace(ctx, default_cs->cmyk);
		fz_drop_colorspace(ctx, default_cs->oi);
		fz_free(ctx, default_cs);
	}
}