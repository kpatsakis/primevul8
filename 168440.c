std_conv_color(fz_context *ctx, fz_color_converter *cc, float *dstv, const float *srcv)
{
	float rgb[3];
	int i;
	const fz_colorspace *srcs = cc->ss;
	const fz_colorspace *dsts = cc->ds;

	if (srcs == NULL)
		srcs = fz_device_rgb(ctx);
	if (dsts == NULL)
		dsts = fz_device_rgb(ctx);

	if (srcs != dsts)
	{
		assert(srcs->to_ccs && dsts->from_ccs);
		srcs->to_ccs(ctx, srcs, srcv, rgb);
		dsts->from_ccs(ctx, dsts, rgb, dstv);
		for (i = 0; i < dsts->n; i++)
			dstv[i] = fz_clamp(dstv[i], 0, 1);
	}
	else
	{
		for (i = 0; i < srcs->n; i++)
			dstv[i] = srcv[i];
	}
}