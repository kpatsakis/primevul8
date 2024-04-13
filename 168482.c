int fz_colorspace_is_subtractive(fz_context *ctx, const fz_colorspace *cs)
{
	return cs && (cs->type == FZ_COLORSPACE_CMYK || cs->type == FZ_COLORSPACE_SEPARATION);
}