static void rgb_to_bgr(fz_context *ctx, const fz_colorspace *cs, const float *rgb, float *bgr)
{
	bgr[0] = rgb[2];
	bgr[1] = rgb[1];
	bgr[2] = rgb[0];
}