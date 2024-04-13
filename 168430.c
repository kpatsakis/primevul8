fz_set_default_gray(fz_context *ctx, fz_default_colorspaces *default_cs, fz_colorspace *cs)
{
	if (cs->n == 1)
	{
		fz_drop_colorspace(ctx, default_cs->gray);
		default_cs->gray = fz_keep_colorspace(ctx, cs);
	}
}