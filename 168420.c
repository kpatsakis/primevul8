fz_icc_from_cal(fz_context *ctx, const fz_colorspace *cs)
{
	fz_cal_colorspace *cal_data = cs->data;
	fz_iccprofile *profile;

	if (cal_data->profile != NULL)
		return cal_data->profile;
	profile = fz_malloc_struct(ctx, fz_iccprofile);

	fz_try(ctx)
	{
		profile->buffer = fz_new_icc_data_from_cal_colorspace(ctx, cal_data);
		fz_md5_icc(ctx, profile);
		cal_data->profile = profile;
	}
	fz_catch(ctx)
	{
		fz_free(ctx, profile);
		fz_rethrow(ctx);
	}

	return profile;
}