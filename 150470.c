struct MACH0_(obj_t) *MACH0_(mach0_new)(const char *file, struct MACH0_(opts_t) *options) {
	struct MACH0_(obj_t) *bin = R_NEW0 (struct MACH0_(obj_t));
	if (!bin) {
		return NULL;
	}
	if (options) {
		bin->verbose = options->verbose;
		bin->header_at = options->header_at;
		bin->symbols_off = options->symbols_off;
	}
	bin->file = file;
	size_t binsz;
	ut8 *buf = (ut8 *)r_file_slurp (file, &binsz);
	bin->size = binsz;
	if (!buf) {
		return MACH0_(mach0_free)(bin);
	}
	bin->b = r_buf_new ();
	if (!r_buf_set_bytes (bin->b, buf, bin->size)) {
		free (buf);
		return MACH0_(mach0_free)(bin);
	}
	free (buf);
	bin->dyld_info = NULL;
	if (!init (bin)) {
		return MACH0_(mach0_free)(bin);
	}
	bin->imports_by_ord_size = 0;
	bin->imports_by_ord = NULL;
	bin->imports_by_name = ht_pp_new ((HtPPDupValue)duplicate_ptr, free_only_key, (HtPPCalcSizeV)ptr_size);
	return bin;
}