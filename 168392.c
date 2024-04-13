int fz_colorspace_n(fz_context *ctx, const fz_colorspace *cs)
{
	return cs ? cs->n : 0;
}