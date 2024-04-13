proto_register_prefix(const char *prefix, prefix_initializer_t pi ) {
	if (! prefixes ) {
		prefixes = g_hash_table_new(prefix_hash, prefix_equal);
	}

	g_hash_table_insert(prefixes, (gpointer)prefix, (gpointer)pi);
}