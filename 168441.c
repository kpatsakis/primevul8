fz_new_colorspace(fz_context *ctx,
		const char *name,
		enum fz_colorspace_type type, int flags, int n,
		fz_colorspace_convert_fn *to_ccs,
		fz_colorspace_convert_fn *from_ccs,
		fz_colorspace_base_fn *base,
		fz_colorspace_clamp_fn *clamp,
		fz_colorspace_destruct_fn *destruct,
		void *data, size_t size)
{
	fz_colorspace *cs = fz_malloc_struct(ctx, fz_colorspace);
	FZ_INIT_KEY_STORABLE(cs, 1, fz_drop_colorspace_imp);
	cs->size = sizeof(fz_colorspace) + size;
	fz_strlcpy(cs->name, name ? name : "UNKNOWN", sizeof cs->name);
	cs->type = type;
	cs->flags = flags;
	cs->n = n;
	cs->to_ccs = to_ccs;
	cs->from_ccs = from_ccs;
	cs->get_base = base;

	if (clamp != NULL)
		cs->clamp = clamp;
	else
		cs->clamp = clamp_default;

	cs->free_data = destruct;
	cs->data = data;
	return cs;
}