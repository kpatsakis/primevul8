fz_pixmap_converter *fz_lookup_pixmap_converter(fz_context *ctx, fz_colorspace *ds, fz_colorspace *ss)
{
	if (ds == NULL)
		return fast_any_to_alpha;

	if (ss == default_gray)
	{
		if (ds == default_rgb) return fast_gray_to_rgb;
		else if (ds == default_bgr) return fast_gray_to_rgb; /* bgr == rgb here */
		else if (ds == default_cmyk) return fast_gray_to_cmyk;
		else return std_conv_pixmap;
	}

	else if (ss == default_rgb)
	{
		if (ds == default_gray) return fast_rgb_to_gray;
		else if (ds == default_bgr) return fast_rgb_to_bgr;
		else if (ds == default_cmyk) return fast_rgb_to_cmyk;
		else return std_conv_pixmap;
	}

	else if (ss == default_bgr)
	{
		if (ds == default_gray) return fast_bgr_to_gray;
		else if (ds == default_rgb) return fast_rgb_to_bgr; /* bgr = rgb here */
		else if (ds == default_cmyk) return fast_bgr_to_cmyk;
		else return std_conv_pixmap;
	}

	else if (ss == default_cmyk)
	{
		if (ds == default_gray) return fast_cmyk_to_gray;
		else if (ds == default_bgr) return fast_cmyk_to_bgr;
		else if (ds == default_rgb) return fast_cmyk_to_rgb;
		else return std_conv_pixmap;
	}
	else if ((ss == fz_device_rgb(ctx) && ds == fz_device_bgr(ctx)) ||
		(ds == fz_device_rgb(ctx) && ss == fz_device_bgr(ctx)))
	{
		return fast_rgb_to_bgr;
	}
	else
	{
		const fz_colorspace *ss_base = fz_source_colorspace_cm(ctx, ss);
		if (ss_base != NULL && fz_colorspace_is_icc(ctx, ds))
		{
			if (ss_base == ss)
				return icc_conv_pixmap;
			else
				return icc_base_conv_pixmap;
		}
		else return std_conv_pixmap;
	}
}