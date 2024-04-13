fz_drop_colorspace_imp(fz_context *ctx, fz_storable *cs_)
{
	fz_colorspace *cs = (fz_colorspace *)cs_;
	int i;

	if (cs->free_data && cs->data)
		cs->free_data(ctx, cs);
	for (i = 0; i < FZ_MAX_COLORS; i++)
		fz_free(ctx, cs->colorant[i]);
	fz_free(ctx, cs);
}