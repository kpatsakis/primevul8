fz_cmm_init_link(fz_context *ctx, fz_icclink *link, const fz_iccprofile *dst, int dst_extras, const fz_iccprofile *src, int src_extras, const fz_iccprofile *prf, const fz_color_params *rend, int cmm_flags, int num_bytes, int copy_spots)
{
	if (ctx && ctx->colorspace && ctx->colorspace->cmm && ctx->cmm_instance)
		ctx->colorspace->cmm->init_link(ctx->cmm_instance, link, dst, dst_extras, src, src_extras, prf, rend, cmm_flags, num_bytes, copy_spots);
}