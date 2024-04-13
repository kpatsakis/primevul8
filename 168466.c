icc_conv_pixmap(fz_context *ctx, fz_pixmap *dst, fz_pixmap *src, fz_colorspace *prf, const fz_default_colorspaces *default_cs, const fz_color_params *color_params, int copy_spots)
{
	fz_colorspace *srcs = src->colorspace;
	fz_colorspace *dsts = dst->colorspace;
	fz_icclink *link;
	int i;
	unsigned char *inputpos, *outputpos;
	int src_n;

	/* Handle DeviceGray to CMYK as K only. See note in Section 6.3 of PDF spec 1.7. */
	if (fz_colorspace_is_device_gray(ctx, srcs) && fz_colorspace_is_cmyk(ctx, dsts))
	{
		fast_gray_to_cmyk(ctx, dst, src, prf, default_cs, color_params, copy_spots);
		return;
	}

	/* Check if we have to do a color space default substitution */
	if (default_cs)
	{
		switch (fz_colorspace_type(ctx, src->colorspace))
		{
		case FZ_COLORSPACE_GRAY:
			if (src->colorspace == fz_device_gray(ctx))
				srcs = fz_default_gray(ctx, default_cs);
			break;
		case FZ_COLORSPACE_RGB:
			if (src->colorspace == fz_device_rgb(ctx))
				srcs = fz_default_rgb(ctx, default_cs);
			break;
		case FZ_COLORSPACE_CMYK:
			if (src->colorspace == fz_device_cmyk(ctx))
				srcs = fz_default_cmyk(ctx, default_cs);
			break;
		default:
			break;
		}
	}

	inputpos = src->samples;
	outputpos = dst->samples;

	link = fz_get_icc_link(ctx, dsts, dst->s + dst->alpha, srcs, src->s + src->alpha, prf, color_params, 1, copy_spots, &src_n);

	if (link->is_identity)
	{
		for (i = 0; i < src->h; i++)
		{
			memcpy(outputpos, inputpos, src->stride);
			inputpos = inputpos + src->stride;
			outputpos = outputpos + dst->stride;
		}
	}
	else
		fz_cmm_transform_pixmap(ctx, link, dst, src);

	fz_drop_icclink(ctx, link);
}