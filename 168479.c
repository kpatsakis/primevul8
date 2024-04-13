int fz_colorspace_is_device(fz_context *ctx, const fz_colorspace *cs)
{
	return cs && (cs->flags & FZ_COLORSPACE_IS_DEVICE);
}