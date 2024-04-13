fz_default_cmyk(fz_context *ctx, const fz_default_colorspaces *default_cs)
{
	if (default_cs)
		return default_cs->cmyk;
	else
		return fz_device_cmyk(ctx);
}