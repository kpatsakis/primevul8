free_cal(fz_context *ctx, fz_colorspace *cs)
{
	fz_cal_colorspace *cal_data = cs->data;
	if (cal_data->profile != NULL)
	{
		fz_drop_buffer(ctx, cal_data->profile->buffer);
		fz_cmm_fin_profile(ctx, cal_data->profile);
		fz_free(ctx, cal_data->profile);
	}
	fz_free(ctx, cal_data);
}