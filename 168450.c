clamp_default(const fz_colorspace *cs, const float *src, float *dst)
{
	int i;

	for (i = 0; i < cs->n; i++)
		dst[i] = fz_clamp(src[i], 0, 1);
}