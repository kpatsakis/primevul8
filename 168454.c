fz_new_default_colorspaces(fz_context *ctx)
{
	fz_default_colorspaces *default_cs = fz_malloc_struct(ctx, fz_default_colorspaces);
	default_cs->refs = 1;
	default_cs->gray = fz_keep_colorspace(ctx, fz_device_gray(ctx));
	default_cs->rgb = fz_keep_colorspace(ctx, fz_device_rgb(ctx));
	default_cs->cmyk = fz_keep_colorspace(ctx, fz_device_cmyk(ctx));
	default_cs->oi = NULL;
	return default_cs;
}