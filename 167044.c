proto_register_protocol(const char *name, const char *short_name,
			const char *filter_name)
{
	protocol_t *protocol;
	header_field_info *hfinfo;

	/*
	 * Make sure there's not already a protocol with any of those
	 * names.  Crash if there is, as that's an error in the code
	 * or an inappropriate plugin.
	 * This situation has to be fixed to not register more than one
	 * protocol with the same name.
	 */

	if (g_hash_table_lookup(proto_names, name)) {
		/* g_error will terminate the program */
		g_error("Duplicate protocol name \"%s\"!"
			" This might be caused by an inappropriate plugin or a development error.", name);
	}

	if (g_hash_table_lookup(proto_short_names, short_name)) {
		g_error("Duplicate protocol short_name \"%s\"!"
			" This might be caused by an inappropriate plugin or a development error.", short_name);
	}

	check_valid_filter_name_or_fail(filter_name);

	if (g_hash_table_lookup(proto_filter_names, filter_name)) {
		g_error("Duplicate protocol filter_name \"%s\"!"
			" This might be caused by an inappropriate plugin or a development error.", filter_name);
	}

	/*
	 * Add this protocol to the list of known protocols;
	 * the list is sorted by protocol short name.
	 */
	protocol = g_new(protocol_t, 1);
	protocol->name = name;
	protocol->short_name = short_name;
	protocol->filter_name = filter_name;
	protocol->fields = NULL; /* Delegate until actually needed */
	protocol->is_enabled = TRUE; /* protocol is enabled by default */
	protocol->enabled_by_default = TRUE; /* see previous comment */
	protocol->can_toggle = TRUE;
	protocol->parent_proto_id = -1;
	protocol->heur_list = NULL;

	/* List will be sorted later by name, when all protocols completed registering */
	protocols = g_list_prepend(protocols, protocol);
	g_hash_table_insert(proto_names, (gpointer)name, protocol);
	g_hash_table_insert(proto_filter_names, (gpointer)filter_name, protocol);
	g_hash_table_insert(proto_short_names, (gpointer)short_name, protocol);

	/* Here we allocate a new header_field_info struct */
	hfinfo = g_slice_new(header_field_info);
	hfinfo->name = name;
	hfinfo->abbrev = filter_name;
	hfinfo->type = FT_PROTOCOL;
	hfinfo->display = BASE_NONE;
	hfinfo->strings = protocol;
	hfinfo->bitmask = 0;
	hfinfo->ref_type = HF_REF_TYPE_NONE;
	hfinfo->blurb = NULL;
	hfinfo->parent = -1; /* This field differentiates protos and fields */

	protocol->proto_id = proto_register_field_init(hfinfo, hfinfo->parent);
	return protocol->proto_id;
}