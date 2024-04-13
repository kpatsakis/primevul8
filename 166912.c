proto_initialize_all_prefixes(void) {
	g_hash_table_foreach_remove(prefixes, initialize_prefix, NULL);
}