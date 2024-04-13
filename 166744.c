proto_register_protocol_in_name_only(const char *name, const char *short_name, const char *filter_name, int parent_proto, enum ftenum field_type)
{
	protocol_t *protocol;
	header_field_info *hfinfo;

	/*
	 * Helper protocols don't need the strict rules as a "regular" protocol
	 * Just register it in a list and make a hf_ field from it
	 */
	if ((field_type != FT_PROTOCOL) && (field_type != FT_BYTES)) {
		g_error("Pino \"%s\" must be of type FT_PROTOCOL or FT_BYTES.", name);
	}

	if (parent_proto < 0) {
		g_error("Must have a valid parent protocol for helper protocol \"%s\"!"
			" This might be caused by an inappropriate plugin or a development error.", name);
	}

	check_valid_filter_name_or_fail(filter_name);

	/* Add this protocol to the list of helper protocols (just so it can be properly freed) */
	protocol = g_new(protocol_t, 1);
	protocol->name = name;
	protocol->short_name = short_name;
	protocol->filter_name = filter_name;
	protocol->fields = NULL; /* Delegate until actually needed */

	/* Enabling and toggling is really determined by parent protocol,
	   but provide default values here */
	protocol->is_enabled = TRUE;
	protocol->enabled_by_default = TRUE;
	protocol->can_toggle = TRUE;

	protocol->parent_proto_id = parent_proto;
	protocol->heur_list = NULL;

	/* List will be sorted later by name, when all protocols completed registering */
	protocols = g_list_prepend(protocols, protocol);

	/* Here we allocate a new header_field_info struct */
	hfinfo = g_slice_new(header_field_info);
	hfinfo->name = name;
	hfinfo->abbrev = filter_name;
	hfinfo->type = field_type;
	hfinfo->display = BASE_NONE;
	if (field_type == FT_BYTES) {
		hfinfo->display |= (BASE_NO_DISPLAY_VALUE|BASE_PROTOCOL_INFO);
	}
	hfinfo->strings = protocol;
	hfinfo->bitmask = 0;
	hfinfo->ref_type = HF_REF_TYPE_NONE;
	hfinfo->blurb = NULL;
	hfinfo->parent = -1; /* This field differentiates protos and fields */

	protocol->proto_id = proto_register_field_init(hfinfo, hfinfo->parent);
	return protocol->proto_id;
}