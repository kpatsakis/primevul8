fz_set_default_output_intent(fz_context *ctx, fz_default_colorspaces *default_cs, fz_colorspace *cs)
{
	fz_drop_colorspace(ctx, default_cs->oi);
	default_cs->oi = fz_keep_colorspace(ctx, cs);

	switch (cs->n)
	{
	case 1:
		if (default_cs->gray == fz_device_gray(ctx))
		{
			fz_drop_colorspace(ctx, default_cs->gray);
			default_cs->gray = fz_keep_colorspace(ctx, cs);
		}
		break;
	case 3:
		if (default_cs->rgb == fz_device_rgb(ctx))
		{
			fz_drop_colorspace(ctx, default_cs->rgb);
			default_cs->rgb = fz_keep_colorspace(ctx, cs);
		}
		break;
	case 4:
		if (default_cs->cmyk == fz_device_cmyk(ctx))
		{
			fz_drop_colorspace(ctx, default_cs->cmyk);
			default_cs->cmyk = fz_keep_colorspace(ctx, cs);
		}
		break;
	}
}