icc_conv_color(fz_context *ctx, fz_color_converter *cc, float *dstv, const float *srcv)
{
	const fz_colorspace *dsts = cc->ds;
	int src_n = cc->n;
	int dst_n = dsts->n;

	fz_icclink *link = (fz_icclink *)cc->link;
	int i;
	unsigned short dstv_s[FZ_MAX_COLORS];
	unsigned short srcv_s[FZ_MAX_COLORS];

	/* Special case.  Link is NULL if we are doing DeviceGray to CMYK */
	if (link == NULL)
	{
		dstv[0] = 0;
		dstv[1] = 0;
		dstv[2] = 0;
		dstv[3] = 1 - srcv[0];
	}
	else if (link->is_identity)
	{
		for (i = 0; i < src_n; i++)
			dstv[i] = srcv[i];
	}
	else
	{
		for (i = 0; i < src_n; i++)
			srcv_s[i] = srcv[i] * 65535;
		fz_cmm_transform_color(ctx, link, dstv_s, srcv_s);
		for (i = 0; i < dst_n; i++)
			dstv[i] = fz_clamp((float) dstv_s[i] / 65535.0f, 0, 1);
	}
}