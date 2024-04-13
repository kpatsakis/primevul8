fz_clone_default_colorspaces(fz_context *ctx, fz_default_colorspaces *base)
{
	fz_default_colorspaces *default_cs = fz_malloc_struct(ctx, fz_default_colorspaces);
	default_cs->refs = 1;
	default_cs->gray = fz_keep_colorspace(ctx, base->gray);
	default_cs->rgb = fz_keep_colorspace(ctx, base->rgb);
	default_cs->cmyk = fz_keep_colorspace(ctx, base->cmyk);
	default_cs->oi = fz_keep_colorspace(ctx, base->oi);
	return default_cs;
}