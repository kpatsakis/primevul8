fz_source_colorspace_cm(fz_context *ctx, const fz_colorspace *cs)
{
	while (cs)
	{
		if (fz_colorspace_is_icc(ctx, cs))
			return cs;
		if (fz_colorspace_is_cal(ctx, cs))
			return cs;
		cs = fz_colorspace_base(ctx, cs);
	}
	return NULL;
}