void fz_find_color_converter(fz_context *ctx, fz_color_converter *cc, const fz_colorspace *is, const fz_colorspace *ds, const fz_colorspace *ss, const fz_color_params *params)
{
	if (ds == NULL)
		ds = fz_device_gray(ctx);
	if (ss == NULL)
		ss = fz_device_gray(ctx);
	cc->ds = ds;
	cc->ss = ss;
	cc->is = is;
	cc->link = NULL;
	if (ss == default_gray)
	{
		if ((ds == default_rgb) || (ds == default_bgr))
			cc->convert = g2rgb;
		else if (ds == default_cmyk)
			cc->convert = g2cmyk;
		else
			cc->convert = std_conv_color;
	}

	else if (ss == default_rgb)
	{
		if (ds == default_gray)
			cc->convert = rgb2g;
		else if (ds == default_bgr)
			cc->convert = rgb2bgr;
		else if (ds == default_cmyk)
			cc->convert = rgb2cmyk;
		else
			cc->convert = std_conv_color;
	}

	else if (ss == default_bgr)
	{
		if (ds == default_gray)
			cc->convert = bgr2g;
		else if (ds == default_rgb)
			cc->convert = rgb2bgr;
		else if (ds == default_cmyk)
			cc->convert = bgr2cmyk;
		else
			cc->convert = std_conv_color;
	}

	else if (ss == default_cmyk)
	{
		if (ds == default_gray)
			cc->convert = cmyk2g;
		else if (ds == default_rgb)
			cc->convert = cmyk2rgb;
		else if (ds == default_bgr)
			cc->convert = cmyk2bgr;
		else
			cc->convert = std_conv_color;
	}
	else if ((ss == fz_device_rgb(ctx) && ds == fz_device_bgr(ctx)) ||
		(ss == fz_device_bgr(ctx) && ds == fz_device_rgb(ctx)))
	{
		cc->convert = rgb2bgr;
	}
	else
	{
		const fz_colorspace *ss_base = fz_source_colorspace_cm(ctx, ss);
		if (ss_base != NULL && fz_colorspace_is_icc(ctx, ds))
		{
			if (ss_base == ss)
				cc->convert = icc_conv_color;
			else
				cc->convert = icc_base_conv_color;

			/* Special case: Do not set link if we are doing DeviceGray to CMYK. */
			/* Handle DeviceGray to CMYK as K only. See note in Section 6.3 of PDF spec 1.7. */
			if (!(fz_colorspace_is_device_gray(ctx, ss_base) && fz_colorspace_is_cmyk(ctx, ds)))
				cc->link = fz_get_icc_link(ctx, ds, 0, ss_base, 0, is, params, 2, 0, &cc->n);
		}
		else
			cc->convert = std_conv_color;
	}
}