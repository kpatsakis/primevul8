int fz_colorspace_device_n_has_only_cmyk(fz_context *ctx, const fz_colorspace *cs)
{
	return cs && ((cs->flags & FZ_CS_HAS_CMYK_AND_SPOTS) == FZ_CS_HAS_CMYK);
}