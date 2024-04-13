static void r_coresym_cache_element_line_info_fini(RCoreSymCacheElementLineInfo *line) {
	if (line) {
		r_coresym_cache_element_flc_fini (&line->flc);
	}
}