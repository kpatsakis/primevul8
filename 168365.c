int fz_colorspace_is_device_cmyk(fz_context *ctx, const fz_colorspace *cs)
{
	return fz_colorspace_is_device(ctx, cs) && fz_colorspace_is_cmyk(ctx, cs);
}