fz_default_gray(fz_context *ctx, const fz_default_colorspaces *default_cs)
{
	if (default_cs)
		return default_cs->gray;
	else
		return fz_device_gray(ctx);
}