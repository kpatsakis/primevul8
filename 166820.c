proto_deregister_field (const int parent, gint hf_id)
{
	header_field_info *hfi;
	protocol_t       *proto;
	guint             i;

	g_free(last_field_name);
	last_field_name = NULL;

	if (hf_id == -1 || hf_id == 0)
		return;

	proto = find_protocol_by_id (parent);
	if (!proto || proto->fields == NULL) {
		return;
	}

	for (i = 0; i < proto->fields->len; i++) {
		hfi = (header_field_info *)g_ptr_array_index(proto->fields, i);
		if (hfi->id == hf_id) {
			/* Found the hf_id in this protocol */
			g_hash_table_steal(gpa_name_map, hfi->abbrev);
			g_ptr_array_remove_index_fast(proto->fields, i);
			g_ptr_array_add(deregistered_fields, gpa_hfinfo.hfi[hf_id]);
			return;
		}
	}
}