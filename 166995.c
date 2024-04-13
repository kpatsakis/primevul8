proto_registrar_get_byname(const char *field_name)
{
	header_field_info    *hfinfo;
	prefix_initializer_t  pi;

	if (!field_name)
		return NULL;

	if (g_strcmp0(field_name, last_field_name) == 0) {
		return last_hfinfo;
	}

	hfinfo = (header_field_info *)g_hash_table_lookup(gpa_name_map, field_name);

	if (hfinfo) {
		g_free(last_field_name);
		last_field_name = g_strdup(field_name);
		last_hfinfo = hfinfo;
		return hfinfo;
	}

	if (!prefixes)
		return NULL;

	if ((pi = (prefix_initializer_t)g_hash_table_lookup(prefixes, field_name) ) != NULL) {
		pi(field_name);
		g_hash_table_remove(prefixes, field_name);
	} else {
		return NULL;
	}

	hfinfo = (header_field_info *)g_hash_table_lookup(gpa_name_map, field_name);

	if (hfinfo) {
		g_free(last_field_name);
		last_field_name = g_strdup(field_name);
		last_hfinfo = hfinfo;
	}
	return hfinfo;
}