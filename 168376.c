std_conv_pixmap(fz_context *ctx, fz_pixmap *dst, fz_pixmap *src, fz_colorspace *prf, const fz_default_colorspaces *default_cs, const fz_color_params *color_params, int copy_spots)
{
	float srcv[FZ_MAX_COLORS];
	float dstv[FZ_MAX_COLORS];
	int srcn, dstn;
	int k, i;
	size_t w = src->w;
	int h = src->h;
	ptrdiff_t d_line_inc = dst->stride - w * dst->n;
	ptrdiff_t s_line_inc = src->stride - w * src->n;
	int da = dst->alpha;
	int sa = src->alpha;

	fz_colorspace *ss = src->colorspace;
	fz_colorspace *ds = dst->colorspace;

	unsigned char *s = src->samples;
	unsigned char *d = dst->samples;

	if ((int)w < 0 || h < 0)
		return;

	if (color_params == NULL)
		color_params = fz_default_color_params(ctx);

	srcn = ss->n;
	dstn = ds->n;

	assert(src->w == dst->w && src->h == dst->h);
	assert(src->n == srcn + sa);
	assert(dst->n == dstn + da);

	if (d_line_inc == 0 && s_line_inc == 0)
	{
		w *= h;
		h = 1;
	}

	/* Special case for Lab colorspace (scaling of components to float) */
	if ((fz_colorspace_is_lab(ctx, ss) || fz_colorspace_is_lab_icc(ctx, ss)) && srcn == 3)
	{
		fz_color_converter cc;

		fz_find_color_converter(ctx, &cc, NULL, ds, ss, color_params);
		while (h--)
		{
			size_t ww = w;
			while (ww--)
			{
				srcv[0] = *s++ / 255.0f * 100;
				srcv[1] = *s++ - 128;
				srcv[2] = *s++ - 128;

				cc.convert(ctx, &cc, dstv, srcv);

				for (k = 0; k < dstn; k++)
					*d++ = dstv[k] * 255;
				if (da)
					*d++ = (sa ? *s : 255);
				s += sa;
			}
			d += d_line_inc;
			s += s_line_inc;
		}
		fz_drop_color_converter(ctx, &cc);
	}

	/* Brute-force for small images */
	else if (w*h < 256)
	{
		fz_color_converter cc;

		fz_find_color_converter(ctx, &cc, NULL, ds, ss, color_params);
		while (h--)
		{
			size_t ww = w;
			while (ww--)
			{
				for (k = 0; k < srcn; k++)
					srcv[k] = *s++ / 255.0f;

				cc.convert(ctx, &cc, dstv, srcv);

				for (k = 0; k < dstn; k++)
					*d++ = dstv[k] * 255;
				if (da)
					*d++ = (sa ? *s : 255);
				s += sa;
			}
			d += d_line_inc;
			s += s_line_inc;
		}
		fz_drop_color_converter(ctx, &cc);
	}

	/* 1-d lookup table for separation and similar colorspaces */
	else if (srcn == 1)
	{
		unsigned char lookup[FZ_MAX_COLORS * 256];
		fz_color_converter cc;

		fz_find_color_converter(ctx, &cc, NULL, ds, ss, color_params);
		for (i = 0; i < 256; i++)
		{
			srcv[0] = i / 255.0f;
			cc.convert(ctx, &cc, dstv, srcv);
			for (k = 0; k < dstn; k++)
				lookup[i * dstn + k] = dstv[k] * 255;
		}
		fz_drop_color_converter(ctx, &cc);

		while (h--)
		{
			size_t ww = w;
			while (ww--)
			{
				i = *s++;
				for (k = 0; k < dstn; k++)
					*d++ = lookup[i * dstn + k];
				if (da)
					*d++ = (sa ? *s : 255);
				s += sa;
			}
			d += d_line_inc;
			s += s_line_inc;
		}
	}

	/* Memoize colors using a hash table for the general case */
	else
	{
		fz_hash_table *lookup;
		unsigned char *color;
		unsigned char dummy = s[0] ^ 255;
		unsigned char *sold = &dummy;
		unsigned char *dold;
		fz_color_converter cc;

		lookup = fz_new_hash_table(ctx, 509, srcn, -1, NULL);
		fz_find_color_converter(ctx, &cc, NULL, ds, ss, color_params);

		fz_try(ctx)
		{
			while (h--)
			{
				size_t ww = w;
				while (ww--)
				{
					if (*s == *sold && memcmp(sold,s,srcn) == 0)
					{
						sold = s;
						memcpy(d, dold, dstn);
						d += dstn;
						s += srcn;
						if (da)
							*d++ = (sa ? *s : 255);
						s += sa;
					}
					else
					{
						sold = s;
						dold = d;
						color = fz_hash_find(ctx, lookup, s);
						if (color)
						{
							memcpy(d, color, dstn);
							s += srcn;
							d += dstn;
							if (da)
								*d++ = (sa ? *s : 255);
							s += sa;
						}
						else
						{
							for (k = 0; k < srcn; k++)
								srcv[k] = *s++ / 255.0f;
							cc.convert(ctx, &cc, dstv, srcv);
							for (k = 0; k < dstn; k++)
								*d++ = dstv[k] * 255;

							fz_hash_insert(ctx, lookup, s - srcn, d - dstn);

							if (da)
								*d++ = (sa ? *s : 255);
							s += sa;
						}
					}
				}
				d += d_line_inc;
				s += s_line_inc;
			}
		}
		fz_always(ctx)
			fz_drop_color_converter(ctx, &cc);
		fz_catch(ctx)
			fz_rethrow(ctx);

		fz_drop_hash_table(ctx, lookup);
	}
}