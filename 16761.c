static void r_coresym_cache_element_flc_fini(RCoreSymCacheElementFLC *flc) {
	if (flc) {
		free (flc->file);
	}
}