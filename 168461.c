const char *fz_colorspace_colorant(fz_context *ctx, const fz_colorspace *cs, int i)
{
	if (!cs || i < 0 || i >= cs->n)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Colorant out of range");

	return cs->colorant[i];
}