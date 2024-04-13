rgb2cmyk(fz_context *ctx, fz_color_converter *cc, float *dv, const float *sv)
{
	float c = 1 - sv[0];
	float m = 1 - sv[1];
	float y = 1 - sv[2];
	float k = fz_min(c, fz_min(m, y));
	dv[0] = c - k;
	dv[1] = m - k;
	dv[2] = y - k;
	dv[3] = k;
}