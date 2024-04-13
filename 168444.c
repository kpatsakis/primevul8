free_icc(fz_context *ctx, fz_colorspace *cs)
{
	fz_iccprofile *profile = cs->data;
	fz_drop_buffer(ctx, profile->buffer);
	fz_cmm_fin_profile(ctx, profile);
	fz_free(ctx, profile);
}