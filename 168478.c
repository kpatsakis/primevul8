fz_expand_indexed_pixmap(fz_context *ctx, const fz_pixmap *src, int alpha)
{
	struct indexed *idx;
	fz_pixmap *dst;
	const unsigned char *s;
	unsigned char *d;
	int y, x, k, n, high;
	unsigned char *lookup;
	fz_irect bbox;
	int s_line_inc, d_line_inc;

	assert(src->colorspace->to_ccs == indexed_to_rgb || src->colorspace->to_ccs == indexed_to_alt);
	assert(src->n == 1 + alpha);

	idx = src->colorspace->data;
	high = idx->high;
	lookup = idx->lookup;
	n = idx->base->n;

	dst = fz_new_pixmap_with_bbox(ctx, idx->base, fz_pixmap_bbox(ctx, src, &bbox), src->seps, alpha);
	s = src->samples;
	d = dst->samples;
	s_line_inc = src->stride - src->w * src->n;
	d_line_inc = dst->stride - dst->w * dst->n;

	if (alpha)
	{
		for (y = 0; y < src->h; y++)
		{
			for (x = 0; x < src->w; x++)
			{
				int v = *s++;
				int a = *s++;
				int aa = a + (a>>7);
				v = fz_mini(v, high);
				for (k = 0; k < n; k++)
					*d++ = (aa * lookup[v * n + k] + 128)>>8;
				*d++ = a;
			}
			s += s_line_inc;
			d += d_line_inc;
		}
	}
	else
	{
		for (y = 0; y < src->h; y++)
		{
			for (x = 0; x < src->w; x++)
			{
				int v = *s++;
				v = fz_mini(v, high);
				for (k = 0; k < n; k++)
					*d++ = lookup[v * n + k];
			}
			s += s_line_inc;
			d += d_line_inc;
		}
	}

	if (src->flags & FZ_PIXMAP_FLAG_INTERPOLATE)
		dst->flags |= FZ_PIXMAP_FLAG_INTERPOLATE;
	else
		dst->flags &= ~FZ_PIXMAP_FLAG_INTERPOLATE;

	return dst;
}