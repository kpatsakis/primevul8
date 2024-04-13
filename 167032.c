proto_get_id_by_filter_name(const gchar *filter_name)
{
	const protocol_t *protocol = NULL;

	DISSECTOR_ASSERT_HINT(filter_name, "No filter name present");

	protocol = (const protocol_t *)g_hash_table_lookup(proto_filter_names, filter_name);

	if (protocol == NULL)
		return -1;
	return protocol->proto_id;
}