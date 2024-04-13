static void r_coresym_cache_element_lined_symbol_fini(RCoreSymCacheElementLinedSymbol *sym) {
	if (sym) {
		r_coresym_cache_element_symbol_fini (&sym->sym);
		r_coresym_cache_element_flc_fini (&sym->flc);
	}
}