fz_colorspace *fz_colorspace_base(fz_context *ctx, const fz_colorspace *cs)
{
	return cs && cs->get_base ? cs->get_base(cs) : NULL;
}