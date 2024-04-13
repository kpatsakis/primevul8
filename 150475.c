struct MACH0_(obj_t) *MACH0_(new_buf)(RBuffer *buf, struct MACH0_(opts_t) *options) {
	r_return_val_if_fail (buf, NULL);
	struct MACH0_(obj_t) *bin = R_NEW0 (struct MACH0_(obj_t));
	if (bin) {
		bin->b = r_buf_ref (buf);
		bin->main_addr = UT64_MAX;
		bin->kv = sdb_new (NULL, "bin.mach0", 0);
		bin->size = r_buf_size (bin->b);
		if (options) {
			bin->verbose = options->verbose;
			bin->header_at = options->header_at;
			bin->symbols_off = options->symbols_off;
		}
		if (!init (bin)) {
			return MACH0_(mach0_free)(bin);
		}
	}
	return bin;
}