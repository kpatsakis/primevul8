int fz_colorspace_is_lab_icc(fz_context *ctx, const fz_colorspace *cs)
{
	return fz_colorspace_is_lab(ctx, cs) && fz_colorspace_is_icc(ctx, cs);
}