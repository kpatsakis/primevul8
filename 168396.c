const char *fz_colorspace_name(fz_context *ctx, const fz_colorspace *cs)
{
	return cs ? cs->name : "";
}