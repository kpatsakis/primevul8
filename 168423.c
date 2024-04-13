int fz_colorspace_device_n_has_cmyk(fz_context *ctx, const fz_colorspace *cs)
{
	return cs && (cs->flags & FZ_CS_HAS_CMYK);
}