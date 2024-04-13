clamp_lab_icc(const fz_colorspace *cs, const float *src, float *dst)
{
	dst[0] = (fz_clamp(src[0], 0, 100)) / 100.0f;
	dst[1] = (fz_clamp(src[1], -128, 127) + 128.0f) / 256;
	dst[2] = (fz_clamp(src[2], -128, 127) + 128.0f) / 256;
}