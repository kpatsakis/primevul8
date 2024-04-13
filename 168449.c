clamp_lab(const fz_colorspace *cs, const float *src, float *dst)
{
	int i;

	for (i = 0; i < 3; i++)
		dst[i] = fz_clamp(src[i], i ? -128 : 0, i ? 127 : 100);
}