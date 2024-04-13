get_base_icc_profile(fz_context *ctx, const fz_colorspace *cs)
{
	fz_colorspace *base;
	fz_cal_colorspace *cal;
	fz_iccprofile *cal_icc;

	if (!cs || !cs->get_base)
		return NULL;

	base = cs->get_base(cs);
	if (base == NULL)
		return NULL;

	if (fz_colorspace_is_icc(ctx, base))
		return base->data;
	if (!fz_colorspace_is_cal(ctx, base))
		return get_base_icc_profile(ctx, base);

	cal = base->data;
	cal_icc = cal->profile;
	if (cal_icc && cal_icc->cmm_handle == NULL)
		fz_cmm_init_profile(ctx, cal_icc);

	return cal_icc;
}