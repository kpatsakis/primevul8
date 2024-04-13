proto_register_fields_section(const int parent, header_field_info *hfi, const int num_records)
{
	int		  i;
	protocol_t	 *proto;

	proto = find_protocol_by_id(parent);

	if (proto->fields == NULL) {
		proto->fields = g_ptr_array_sized_new(num_records);
	}

	for (i = 0; i < num_records; i++) {
		/*
		 * Make sure we haven't registered this yet.
		 */
		if (hfi[i].id != -1) {
			fprintf(stderr,
				"Duplicate field detected in call to proto_register_fields: %s is already registered\n",
				hfi[i].abbrev);
			return;
		}

		proto_register_field_common(proto, &hfi[i], parent);
	}
}