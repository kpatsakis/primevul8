icc_base_conv_color(fz_context *ctx, fz_color_converter *cc, float *dstv, const float *srcv)
{
	const fz_colorspace *srcs = cc->ss;

	float local_src_map[FZ_MAX_COLORS];
	float local_src_map2[FZ_MAX_COLORS];
	float *src_map = local_src_map;

	do
	{
		srcs->to_ccs(ctx, srcs, srcv, src_map);
		srcs = srcs->get_base(srcs);
		srcs->clamp(srcs, src_map, src_map);
		srcv = src_map;
		src_map = (src_map == local_src_map ? local_src_map2 : local_src_map);
	}
	while (!fz_colorspace_is_icc(ctx, srcs) && !fz_colorspace_is_cal(ctx, srcs));

	icc_conv_color(ctx, cc, dstv, srcv);
}