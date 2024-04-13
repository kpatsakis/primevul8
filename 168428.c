int fz_colorspace_is_bgr(fz_context *ctx, const fz_colorspace *cs)
{
	return cs && cs->type == FZ_COLORSPACE_BGR;
}