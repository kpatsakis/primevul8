int fz_colorspace_is_cal(fz_context *ctx, const fz_colorspace *cs)
{
	return cs && (cs->flags & FZ_COLORSPACE_IS_CAL);
}