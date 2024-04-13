fz_new_icc_link(fz_context *ctx, fz_iccprofile *dst, int dst_extras, fz_iccprofile *src, int src_extras, fz_iccprofile *prf, const fz_color_params *rend, int num_bytes, int copy_extras)
{
	fz_icclink *link = fz_malloc_struct(ctx, fz_icclink);
	FZ_INIT_STORABLE(link, 1, fz_drop_link_imp);

	if (memcmp(src->md5, dst->md5, 16) == 0 && prf == NULL)
	{
		link->is_identity = 1;
		return link;
	}

	fz_try(ctx)
		fz_cmm_init_link(ctx, link, dst, dst_extras, src, src_extras, prf, rend, 0, num_bytes, copy_extras);
	fz_catch(ctx)
	{
		fz_free(ctx, link);
		fz_rethrow(ctx);
	}

	return link;
}