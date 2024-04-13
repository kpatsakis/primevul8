convert_to_icc_base(fz_context *ctx, fz_colorspace *srcs, float *src_f, float *des_f)
{
	float temp_f[FZ_MAX_COLORS];
	fz_colorspace *base_cs = srcs->get_base(srcs);

	if (fz_colorspace_is_icc(ctx, base_cs) || fz_colorspace_is_cal(ctx, base_cs) || fz_colorspace_is_lab(ctx, base_cs))
		srcs->to_ccs(ctx, srcs, src_f, des_f);
	else
	{
		srcs->to_ccs(ctx, srcs, src_f, temp_f);
		convert_to_icc_base(ctx, base_cs, temp_f, des_f);
	}
}