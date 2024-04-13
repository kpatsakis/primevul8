proto_name_already_registered(const gchar *name)
{
	DISSECTOR_ASSERT_HINT(name, "No name present");

	if (g_hash_table_lookup(proto_names, name) != NULL)
		return TRUE;
	return FALSE;
}