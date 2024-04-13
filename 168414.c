fz_get_icc_link(fz_context *ctx, const fz_colorspace *dst, int dst_extras, const fz_colorspace *src, int src_extras, const fz_colorspace *prf, const fz_color_params *rend, int num_bytes, int copy_spots, int *src_n)
{
	fz_icclink *link = NULL;
	fz_iccprofile *src_icc = NULL;
	fz_iccprofile *dst_icc = dst->data;
	fz_iccprofile *prf_icc = NULL;
	fz_link_key *key = NULL;
	fz_icclink *new_link;

	assert(!copy_spots || src_extras == dst_extras);

	if (prf != NULL)
		prf_icc = prf->data;

	if (fz_colorspace_is_icc(ctx, src))
		src_icc = src->data;
	else if (fz_colorspace_is_cal(ctx, src))
	{
		fz_cal_colorspace *cal;

		cal = src->data;
		src_icc = cal->profile;
		/* Check if we have any work to do. */
		if (src_icc == NULL)
			src_icc = fz_icc_from_cal(ctx, src);
		if (src_icc->cmm_handle == NULL)
		{
			fz_cmm_init_profile(ctx, src_icc);

			/* The CMM failed to make a profile. Use the default. */
			if (src_icc->cmm_handle == NULL)
			{
				switch (src->n)
				{
				case 1:
					src_icc = fz_device_gray(ctx)->data;
					break;
				case 3:
					src_icc = fz_device_rgb(ctx)->data;
					break;
				case 4:
					src_icc = fz_device_cmyk(ctx)->data;
					break;
				default:
					fz_throw(ctx, FZ_ERROR_GENERIC, "Poorly formed Cal color space");
				}
				/* To avoid repeated failures building the pdf-cal color space,
				 * assign the default profile. */
				fz_cmm_fin_profile(ctx, src_icc);
				cal->profile = src_icc;
			}
		}
	}
	else
		src_icc = get_base_icc_profile(ctx, src);

	if (src_icc == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Profile missing during link creation");

	*src_n = src_icc->num_devcomp;

	fz_var(link);
	fz_var(key);

	if (rend == NULL)
		rend = fz_default_color_params(ctx);

	fz_try(ctx)
	{
		/* Check the storable to see if we have a copy. */
		key = fz_malloc_struct(ctx, fz_link_key);
		key->refs = 1;
		memcpy(&key->dst_md5, dst_icc->md5, 16);
		memcpy(&key->src_md5, src_icc->md5, 16);
		key->rend.ri = rend->ri;
		key->rend.bp = rend->bp;
		key->src_extras = src_extras;
		key->dst_extras = dst_extras;
		key->depth = num_bytes;
		key->proof = (prf_icc != NULL);
		key->copy_spots = copy_spots;
		link = fz_find_item(ctx, fz_drop_link_imp, key, &fz_link_store_type);

		/* Not found.  Make new one add to store. */
		if (link == NULL)
		{
			link = fz_new_icc_link(ctx, dst_icc, dst_extras, src_icc, src_extras, prf_icc, rend, num_bytes, copy_spots);
			new_link = fz_store_item(ctx, key, link, sizeof(fz_icclink), &fz_link_store_type);
			if (new_link != NULL)
			{
				/* Found one while adding! Perhaps from another thread? */
				fz_drop_icclink(ctx, link);
				link = new_link;
			}
		}
	}
	fz_always(ctx)
	{
		fz_drop_link_key(ctx, key);
	}
	fz_catch(ctx)
	{
		/* Ignore any error that came just from the enstoring. */
		if (link == NULL)
			fz_rethrow(ctx);
	}
	return link;
}