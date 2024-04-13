static void r_coresym_cache_element_segment_fini(RCoreSymCacheElementSegment *seg) {
	if (seg) {
		free (seg->name);
	}
}