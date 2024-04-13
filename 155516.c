pdf_drop_material(fz_context *ctx, pdf_material *mat)
{
	fz_drop_colorspace(ctx, mat->colorspace);
	pdf_drop_pattern(ctx, mat->pattern);
	fz_drop_shade(ctx, mat->shade);
	return mat;
}