fz_colorspace *fz_new_icc_colorspace_from_stream(fz_context *ctx, const char *name, fz_stream *in)
{
	fz_colorspace *cs = NULL;
	fz_buffer *buffer = fz_read_all(ctx, in, 1024);
	fz_try(ctx)
		cs = fz_new_icc_colorspace(ctx, name, 0, buffer);
	fz_always(ctx)
		fz_drop_buffer(ctx, buffer);
	fz_catch(ctx)
		fz_rethrow(ctx);
	return cs;
}