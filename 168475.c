int fz_colorspace_is_gray(fz_context *ctx, const fz_colorspace *cs)
{
	return cs && cs->type == FZ_COLORSPACE_GRAY;
}