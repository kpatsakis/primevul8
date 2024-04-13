static void fast_any_to_alpha(fz_context *ctx, fz_pixmap *dst, fz_pixmap *src, fz_colorspace *prf, const fz_default_colorspaces *default_cs, const fz_color_params *color_params, int copy_spots)
{
	assert(copy_spots && dst->s == 0 && src->s == 0);

	if (!src->alpha)
		fz_clear_pixmap_with_value(ctx, dst, 255);
	else
	{
		unsigned char *s = src->samples;
		unsigned char *d = dst->samples;
		size_t w = src->w;
		int h = src->h;
		int n = src->n;
		ptrdiff_t d_line_inc = dst->stride - w * dst->n;
		ptrdiff_t s_line_inc = src->stride - w * src->n;

		if ((int)w < 0 || h < 0)
			return;

		assert(dst->alpha && src->alpha && dst->n == 1);

		if (d_line_inc == 0 && s_line_inc == 0)
		{
			w *= h;
			h = 1;
		}

		s += n-1;
		while (h--)
		{
			size_t ww = w;
			while (ww--)
			{
				*d++ = *s;
				s += n;
			}
			d += d_line_inc;
			s += s_line_inc;
		}
	}
}