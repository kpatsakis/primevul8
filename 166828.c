proto_deregister_protocol(const char *short_name)
{
	protocol_t *protocol;
	header_field_info *hfinfo;
	int proto_id;
	guint i;

	proto_id = proto_get_id_by_short_name(short_name);
	protocol = find_protocol_by_id(proto_id);
	if (protocol == NULL)
		return FALSE;

	g_hash_table_remove(proto_names, protocol->name);
	g_hash_table_remove(proto_short_names, (gpointer)short_name);
	g_hash_table_remove(proto_filter_names, (gpointer)protocol->filter_name);

	if (protocol->fields) {
		for (i = 0; i < protocol->fields->len; i++) {
			hfinfo = (header_field_info *)g_ptr_array_index(protocol->fields, i);
			hfinfo_remove_from_gpa_name_map(hfinfo);
			expert_deregister_expertinfo(hfinfo->abbrev);
			g_ptr_array_add(deregistered_fields, gpa_hfinfo.hfi[hfinfo->id]);
		}
		g_ptr_array_free(protocol->fields, TRUE);
		protocol->fields = NULL;
	}

	g_list_free(protocol->heur_list);

	/* Remove this protocol from the list of known protocols */
	protocols = g_list_remove(protocols, protocol);

	g_ptr_array_add(deregistered_fields, gpa_hfinfo.hfi[proto_id]);
	g_hash_table_steal(gpa_name_map, protocol->filter_name);

	g_free(last_field_name);
	last_field_name = NULL;

	return TRUE;
}