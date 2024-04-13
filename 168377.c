static void gray_to_rgb(fz_context *ctx, const fz_colorspace *cs, const float *gray, float *rgb)
{
	rgb[0] = gray[0];
	rgb[1] = gray[0];
	rgb[2] = gray[0];
}