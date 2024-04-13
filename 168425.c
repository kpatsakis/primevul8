fz_set_default_rgb(fz_context *ctx, fz_default_colorspaces *default_cs, fz_colorspace *cs)
{
	if (cs->n == 3)
	{
		fz_drop_colorspace(ctx, default_cs->rgb);
		default_cs->rgb = fz_keep_colorspace(ctx, cs);
	}
}