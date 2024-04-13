fz_default_rgb(fz_context *ctx, const fz_default_colorspaces *default_cs)
{
	if (default_cs)
		return default_cs->rgb;
	else
		return fz_device_rgb(ctx);
}