fz_clamp_color(fz_context *ctx, const fz_colorspace *cs, const float *in, float *out)
{
	cs->clamp(cs, in, out);
}