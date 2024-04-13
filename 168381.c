g2cmyk(fz_context *ctx, fz_color_converter *cc, float *dv, const float *sv)
{
	dv[0] = 0;
	dv[1] = 0;
	dv[2] = 0;
	dv[3] = 1 - sv[0];
}