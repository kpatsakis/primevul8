char *MACH0_(get_class)(struct MACH0_(obj_t) *bin) {
#if R_BIN_MACH064
	return r_str_new ("MACH064");
#else
	return r_str_new ("MACH0");
#endif
}