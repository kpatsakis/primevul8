rgb2g(fz_context *ctx, fz_color_converter *cc, float *dv, const float *sv)
{
	dv[0] = sv[0] * 0.3f + sv[1] * 0.59f + sv[2] * 0.11f;
}