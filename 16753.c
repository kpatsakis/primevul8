ut64 r_coresym_cache_element_pa2va(RCoreSymCacheElement *element, ut64 pa) {
	size_t i;
	for (i = 0; i < element->hdr->n_segments; i++) {
		RCoreSymCacheElementSegment *seg = &element->segments[i];
		if (seg->size == 0) {
			continue;
		}
		if (seg->paddr < pa && pa < seg->paddr + seg->size) {
			return pa - seg->paddr + seg->vaddr;
		}
	}
	return pa;
}