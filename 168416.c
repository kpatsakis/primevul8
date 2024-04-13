fz_set_default_cmyk(fz_context *ctx, fz_default_colorspaces *default_cs, fz_colorspace *cs)
{
	if (cs->n == 4)
	{
		fz_drop_colorspace(ctx, default_cs->cmyk);
		default_cs->cmyk = fz_keep_colorspace(ctx, cs);
	}
}