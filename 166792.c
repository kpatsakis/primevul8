proto_get_id_by_short_name(const gchar *short_name)
{
	const protocol_t *protocol = NULL;

	DISSECTOR_ASSERT_HINT(short_name, "No short name present");

	protocol = (const protocol_t *)g_hash_table_lookup(proto_short_names, short_name);

	if (protocol == NULL)
		return -1;
	return protocol->proto_id;
}