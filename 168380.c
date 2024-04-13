fz_new_cal_colorspace(fz_context *ctx, const char *name, float *wp, float *bp, float *gamma, float *matrix)
{
	fz_colorspace *cs = NULL;
	enum fz_colorspace_type type;
	int num;
	fz_cal_colorspace *cal_data;

	if (matrix)
	{
		type = FZ_COLORSPACE_RGB;
		num = 3;
	}
	else
	{
		type = FZ_COLORSPACE_GRAY;
		num = 1;
	}

	cal_data = fz_malloc_struct(ctx, fz_cal_colorspace);
	memcpy(&cal_data->bp, bp, sizeof(float) * 3);
	memcpy(&cal_data->wp, wp, sizeof(float) * 3);
	memcpy(&cal_data->gamma, gamma, sizeof(float) * num);
	if (matrix != NULL)
		memcpy(&cal_data->matrix, matrix, sizeof(float) * 9);
	cal_data->n = num;

	fz_try(ctx)
		cs = fz_new_colorspace(ctx, name, type, FZ_COLORSPACE_IS_CAL, num, NULL, NULL, NULL, NULL, free_cal, cal_data, sizeof(cal_data));
	fz_catch(ctx)
	{
		fz_free(ctx, cal_data);
		fz_rethrow(ctx);
	}
	return cs;
}