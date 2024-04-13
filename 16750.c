static void r_coresym_cache_element_section_fini(RCoreSymCacheElementSection *sec) {
	if (sec) {
		free (sec->name);
	}
}