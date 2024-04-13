icc_base_conv_pixmap(fz_context *ctx, fz_pixmap *dst, fz_pixmap *src, fz_colorspace *prf, const fz_default_colorspaces *default_cs, const fz_color_params *color_params, int copy_spots)
{
	fz_colorspace *srcs = src->colorspace;
	fz_colorspace *base_cs = get_base_icc_space(ctx, srcs);
	int i, j;
	unsigned char *inputpos, *outputpos;
	fz_pixmap *base;
	fz_irect bbox;
	int h, len;
	float src_f[FZ_MAX_COLORS], des_f[FZ_MAX_COLORS];
	int sn = src->n;
	int sc = sn - src->alpha - src->s;
	int stride_src = src->stride - src->w * sn;
	int stride_base;
	int bn, bc;

	base = fz_new_pixmap_with_bbox(ctx, base_cs, fz_pixmap_bbox(ctx, src, &bbox), src->seps, src->alpha);
	bn = base->n;
	bc = base->n - base->alpha - base->s;
	stride_base = base->stride - base->w * bn;

	inputpos = src->samples;
	outputpos = base->samples;

	h = src->h;
	while (h--)
	{
		len = src->w;
		while (len--)
		{
			/* Convert the actual colors */
			for (i = 0; i < sc; i++)
				src_f[i] = (float) inputpos[i] / 255.0f;

			convert_to_icc_base(ctx, srcs, src_f, des_f);
			base_cs->clamp(base_cs, des_f, des_f);

			for (j = 0; j < bc; j++)
				outputpos[j] = des_f[j] * 255.0f;
			/* Copy spots and alphas unchanged */
			for (; i < sn; i++, j++)
				outputpos[j] = inputpos[i];

			outputpos += bn;
			inputpos += sn;
		}
		outputpos += stride_base;
		inputpos += stride_src;
	}

	fz_try(ctx)
		icc_conv_pixmap(ctx, dst, base, prf, default_cs, color_params, copy_spots);
	fz_always(ctx)
		fz_drop_pixmap(ctx, base);
	fz_catch(ctx)
		fz_rethrow(ctx);
}