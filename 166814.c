proto_registrar_get_byalias(const char *alias_name)
{
	if (!alias_name) {
		return NULL;
	}

	/* Find our aliased protocol. */
	char *an_copy = g_strdup(alias_name);
	char *dot = strchr(an_copy, '.');
	if (dot) {
		*dot = '\0';
	}
	const char *proto_pfx = (const char *) g_hash_table_lookup(gpa_protocol_aliases, an_copy);
	if (!proto_pfx) {
		g_free(an_copy);
		return NULL;
	}

	/* Construct our aliased field and look it up. */
	GString *filter_name = g_string_new(proto_pfx);
	if (dot) {
		g_string_append_printf(filter_name, ".%s", dot+1);
	}
	header_field_info *hfinfo = proto_registrar_get_byname(filter_name->str);
	g_free(an_copy);
	g_string_free(filter_name, TRUE);

	return hfinfo;
}