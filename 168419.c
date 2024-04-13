static void fast_rgb_to_bgr(fz_context *ctx, fz_pixmap *dst, fz_pixmap *src, fz_colorspace *prf, const fz_default_colorspaces *default_cs, const fz_color_params *color_params, int copy_spots)
{
	unsigned char *s = src->samples;
	unsigned char *d = dst->samples;
	size_t w = src->w;
	int h = src->h;
	int sn = src->n;
	int ss = src->s;
	int sa = src->alpha;
	int dn = dst->n;
	int ds = dst->s;
	int da = dst->alpha;
	ptrdiff_t d_line_inc = dst->stride - w * dn;
	ptrdiff_t s_line_inc = src->stride - w * sn;

	/* If copying spots, they must match, and we can never drop alpha (but we can invent it) */
	if ((copy_spots && ss != ds) || (!da && sa))
	{
		assert("This should never happen" == NULL);
		fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot convert between incompatible pixmaps");
	}

	if ((int)w < 0 || h < 0)
		return;

	if (d_line_inc == 0 && s_line_inc == 0)
	{
		w *= h;
		h = 1;
	}

	if (ss == 0 && ds == 0)
	{
		/* Common, no spots case */
		if (da)
		{
			if (sa)
			{
				while (h--)
				{
					size_t ww = w;
					while (ww--)
					{
						d[0] = s[2];
						d[1] = s[1];
						d[2] = s[0];
						d[3] = s[3];
						s += 4;
						d += 4;
					}
				}
			}
			else
			{
				while (h--)
				{
					size_t ww = w;
					while (ww--)
					{
						d[0] = s[2];
						d[1] = s[1];
						d[2] = s[0];
						d[3] = 255;
						s += 3;
						d += 4;
					}
				}
			}
		}
		else
		{
			while (h--)
			{
				size_t ww = w;
				while (ww--)
				{
					d[0] = s[2];
					d[1] = s[1];
					d[2] = s[0];
					s += 3;
					d += 3;
				}
			}
		}
	}
	else if (copy_spots)
	{
		/* Slower, spots capable version */
		while (h--)
		{
			int i;
			size_t ww = w;
			while (ww--)
			{
				d[0] = s[2];
				d[1] = s[1];
				d[2] = s[0];
				s += 3;
				d += 3;
				for (i=ss; i > 0; i--)
					*d++ = *s++;
				if (da)
					*d++ = sa ? *s++ : 255;
			}
			d += d_line_inc;
			s += s_line_inc;
		}
	}
	else
	{
		while (h--)
		{
			size_t ww = w;
			while (ww--)
			{
				d[0] = s[2];
				d[1] = s[1];
				d[2] = s[0];
				s += sn;
				d += dn;
				if (da)
					d[-1] = sa ? s[-1] : 255;
			}
			d += d_line_inc;
			s += s_line_inc;
		}
	}
}