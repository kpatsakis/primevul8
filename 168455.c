void fz_set_icc_bgr(fz_context *ctx, fz_colorspace *cs)
{
	fz_iccprofile *profile;

	if (cs == NULL || !fz_colorspace_is_icc(ctx, cs))
		return;

	profile = cs->data;
	profile->bgr = 1;
	return;
}