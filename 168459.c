get_base_icc_space(fz_context *ctx, fz_colorspace *srcs)
{
	while (1)
	{
		srcs = srcs->get_base(srcs);
		if (srcs == NULL)
			fz_throw(ctx, FZ_ERROR_GENERIC, "Final color space should be icc or pdf-cal or lab");

		if (fz_colorspace_is_icc(ctx, srcs) || fz_colorspace_is_cal(ctx, srcs) || fz_colorspace_is_lab(ctx, srcs))
			return srcs;
	}
}