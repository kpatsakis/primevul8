indexed_to_alt(fz_context *ctx, const fz_colorspace *cs, const float *color, float *alt)
{
	struct indexed *idx = cs->data;
	int i, k;
	int n = idx->base->n;

	i = color[0] * 255;
	i = fz_clampi(i, 0, idx->high);
	for (k = 0; k < n; k++)
		alt[k] = idx->lookup[i * n + k] / 255.0f;
}