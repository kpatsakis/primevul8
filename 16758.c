void r_coresym_cache_element_free(RCoreSymCacheElement *element) {
	if (!element) {
		return;
	}
	size_t i;
	if (element->segments) {
		for (i = 0; i < element->hdr->n_segments; i++) {
			r_coresym_cache_element_segment_fini (&element->segments[i]);
		}
	}
	if (element->sections) {
		for (i = 0; i < element->hdr->n_sections; i++) {
			r_coresym_cache_element_section_fini (&element->sections[i]);
		}
	}
	if (element->symbols) {
		for (i = 0; i < element->hdr->n_symbols; i++) {
			r_coresym_cache_element_symbol_fini (&element->symbols[i]);
		}
	}
	if (element->lined_symbols) {
		for (i = 0; i < element->hdr->n_lined_symbols; i++) {
			r_coresym_cache_element_lined_symbol_fini (&element->lined_symbols[i]);
		}
	}
	if (element->line_info) {
		for (i = 0; i < element->hdr->n_line_info; i++) {
			r_coresym_cache_element_line_info_fini (&element->line_info[i]);
		}
	}
	free (element->segments);
	free (element->sections);
	free (element->symbols);
	free (element->lined_symbols);
	free (element->line_info);
	free (element->hdr);
	free (element->file_name);
	free (element->binary_version);
	free (element);
}