fz_convert_color(fz_context *ctx, const fz_color_params *params, const fz_colorspace *is, const fz_colorspace *ds, float *dv, const fz_colorspace *ss, const float *sv)
{
	fz_color_converter cc;
	fz_find_color_converter(ctx, &cc, is, ds, ss, params);
	cc.convert(ctx, &cc, dv, sv);
	fz_drop_color_converter(ctx, &cc);
}