proto_register_alias(const int proto_id, const char *alias_name)
{
	protocol_t *protocol;

	protocol = find_protocol_by_id(proto_id);
	if (alias_name && protocol) {
		g_hash_table_insert(gpa_protocol_aliases, (gpointer) alias_name, (gpointer)protocol->filter_name);
	}
}