clamp_default_icc(const fz_colorspace *cs, const float *src, float *dst)
{
	int i;
	fz_iccprofile *profile = cs->data;

	for (i = 0; i < profile->num_devcomp; i++)
		dst[i] = fz_clamp(src[i], 0, 1);
}