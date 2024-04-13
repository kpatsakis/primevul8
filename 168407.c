static void bgr_to_rgb(fz_context *ctx, const fz_colorspace *cs, const float *bgr, float *rgb)
{
	rgb[0] = bgr[2];
	rgb[1] = bgr[1];
	rgb[2] = bgr[0];
}