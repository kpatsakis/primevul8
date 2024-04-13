fz_md5_icc(fz_context *ctx, fz_iccprofile *profile)
{
	if (profile)
		fz_md5_buffer(ctx, profile->buffer, profile->md5);
}