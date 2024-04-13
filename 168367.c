static void rgb_to_rgb(fz_context *ctx, const fz_colorspace *cs, const float *rgb, float *xyz)
{
	xyz[0] = rgb[0];
	xyz[1] = rgb[1];
	xyz[2] = rgb[2];
}