indexed_to_rgb(fz_context *ctx, const fz_colorspace *cs, const float *color, float *rgb)
{
	float alt[FZ_MAX_COLORS];
	struct indexed *idx = cs->data;

	indexed_to_alt(ctx, cs, color, alt);
	idx->base->to_ccs(ctx, idx->base, alt, rgb);
}