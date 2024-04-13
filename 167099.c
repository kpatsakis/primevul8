proto_registrar_dump_elastic(const gchar* filter)
{
	header_field_info *hfinfo;
	header_field_info *parent_hfinfo;
	guint i;
	gboolean open_object = TRUE;
	const char* prev_proto = NULL;
	gchar* str;
	gchar** protos = NULL;
	gchar* proto;
	gboolean found;
	guint j;
	gchar* type;

	/* We have filtering protocols. Extract them. */
	if (filter) {
		protos = g_strsplit(filter, ",", -1);
	}

	/*
	 * To help tracking down the json tree, objects have been appended with a comment:
	 * n.label -> where n is the indentation level and label the name of the object
	 */

	json_dumper dumper = {
		.output_file = stdout,
		.flags = JSON_DUMPER_FLAGS_PRETTY_PRINT,
	};
	json_dumper_begin_object(&dumper); // 1.root
	elastic_add_base_mapping(&dumper);

	json_dumper_set_member_name(&dumper, "mappings");
	json_dumper_begin_object(&dumper); // 2.mappings
	json_dumper_set_member_name(&dumper, "doc");

	json_dumper_begin_object(&dumper); // 3.doc
	json_dumper_set_member_name(&dumper, "dynamic");
	json_dumper_value_anyf(&dumper, "false");

	json_dumper_set_member_name(&dumper, "properties");
	json_dumper_begin_object(&dumper); // 4.properties
	json_dumper_set_member_name(&dumper, "timestamp");
	json_dumper_begin_object(&dumper); // 5.timestamp
	json_dumper_set_member_name(&dumper, "type");
	json_dumper_value_string(&dumper, "date");
	json_dumper_end_object(&dumper); // 5.timestamp

	json_dumper_set_member_name(&dumper, "layers");
	json_dumper_begin_object(&dumper); // 5.layers
	json_dumper_set_member_name(&dumper, "properties");
	json_dumper_begin_object(&dumper); // 6.properties

	for (i = 0; i < gpa_hfinfo.len; i++) {
		if (gpa_hfinfo.hfi[i] == NULL)
			continue; /* This is a deregistered protocol or header field */

		PROTO_REGISTRAR_GET_NTH(i, hfinfo);

		/*
		 * Skip the pseudo-field for "proto_tree_add_text()" since
		 * we don't want it in the list of filterable protocols.
		 */
		if (hfinfo->id == hf_text_only)
			continue;

		if (!proto_registrar_is_protocol(i)) {
			PROTO_REGISTRAR_GET_NTH(hfinfo->parent, parent_hfinfo);

			/*
			 * Skip the field if filter protocols have been set and this one's
			 * parent is not listed.
			 */
			if (protos) {
				found = FALSE;
				j = 0;
				proto = protos[0];
				while(proto) {
					if (!g_strcmp0(proto, parent_hfinfo->abbrev)) {
						found = TRUE;
						break;
					}
					j++;
					proto = protos[j];
				}
				if (!found)
					continue;
			}

			if (prev_proto && g_strcmp0(parent_hfinfo->abbrev, prev_proto)) {
				json_dumper_end_object(&dumper); // 8.properties
				json_dumper_end_object(&dumper); // 7.parent_hfinfo->abbrev
				open_object = TRUE;
			}

			prev_proto = parent_hfinfo->abbrev;

			if (open_object) {
				json_dumper_set_member_name(&dumper, parent_hfinfo->abbrev);
				json_dumper_begin_object(&dumper); // 7.parent_hfinfo->abbrev
				json_dumper_set_member_name(&dumper, "properties");
				json_dumper_begin_object(&dumper); // 8.properties
				open_object = FALSE;
			}
			/* Skip the fields that would map into string. This is the default in elasticsearch. */
			type = ws_type_to_elastic(hfinfo->type);
			if (type) {
				str = g_strdup_printf("%s_%s", prev_proto, hfinfo->abbrev);
				json_dumper_set_member_name(&dumper, dot_to_underscore(str));
				g_free(str);
				json_dumper_begin_object(&dumper); // 9.hfinfo->abbrev
				json_dumper_set_member_name(&dumper, "type");
				json_dumper_value_string(&dumper, type);
				json_dumper_end_object(&dumper); // 9.hfinfo->abbrev
			}
		}
	}

	if (prev_proto) {
		json_dumper_end_object(&dumper); // 8.properties
		json_dumper_end_object(&dumper); // 7.parent_hfinfo->abbrev
	}

	json_dumper_end_object(&dumper); // 6.properties
	json_dumper_end_object(&dumper); // 5.layers
	json_dumper_end_object(&dumper); // 4.properties
	json_dumper_end_object(&dumper); // 3.doc
	json_dumper_end_object(&dumper); // 2.mappings
	json_dumper_end_object(&dumper); // 1.root
	gboolean ret = json_dumper_finish(&dumper);
	DISSECTOR_ASSERT(ret);

	g_strfreev(protos);
}