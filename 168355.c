cmyk2g(fz_context *ctx, fz_color_converter *cc, float *dv, const float *sv)
{
	float c = sv[0] * 0.3f;
	float m = sv[1] * 0.59f;
	float y = sv[2] * 0.11f;
	dv[0] = 1 - fz_min(c + m + y + sv[3], 1);
}