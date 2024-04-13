fz_icc_data_from_icc_colorspace(fz_context *ctx, const fz_colorspace *cs)
{
	fz_iccprofile *profile;

	if (cs == NULL || !fz_colorspace_is_icc(ctx, cs))
		return NULL;
	profile = cs->data;
	if (!profile)
		return NULL;
	return profile->buffer;
}