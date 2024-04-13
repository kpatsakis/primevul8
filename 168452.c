fz_colorspace_type(fz_context *ctx, fz_colorspace *cs)
{
	return cs ? cs->type : FZ_COLORSPACE_NONE;
}