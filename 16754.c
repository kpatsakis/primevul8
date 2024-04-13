static void r_coresym_cache_element_symbol_fini(RCoreSymCacheElementSymbol *sym) {
	if (sym) {
		free (sym->name);
		free (sym->mangled_name);
	}
}