fz_colorspace *fz_new_icc_colorspace_from_file(fz_context *ctx, const char *name, const char *path)
{
	fz_colorspace *cs = NULL;
	fz_buffer *buffer = fz_read_file(ctx, path);
	fz_try(ctx)
		cs = fz_new_icc_colorspace(ctx, name, 0, buffer);
	fz_always(ctx)
		fz_drop_buffer(ctx, buffer);
	fz_catch(ctx)
		fz_rethrow(ctx);
	return cs;
}