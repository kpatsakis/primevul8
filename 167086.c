proto_init(GSList *register_all_plugin_protocols_list,
	   GSList *register_all_plugin_handoffs_list,
	   register_cb cb,
	   gpointer client_data)
{
	proto_cleanup_base();

	proto_names        = g_hash_table_new(g_str_hash, g_str_equal);
	proto_short_names  = g_hash_table_new(g_str_hash, g_str_equal);
	proto_filter_names = g_hash_table_new(g_str_hash, g_str_equal);

	gpa_hfinfo.len           = 0;
	gpa_hfinfo.allocated_len = 0;
	gpa_hfinfo.hfi           = NULL;
	gpa_name_map             = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, save_same_name_hfinfo);
	gpa_protocol_aliases     = g_hash_table_new(g_str_hash, g_str_equal);
	deregistered_fields      = g_ptr_array_new();
	deregistered_data        = g_ptr_array_new();

	/* Initialize the ftype subsystem */
	ftypes_initialize();

	/* Initialize the addres type subsystem */
	address_types_initialize();

	/* Register one special-case FT_TEXT_ONLY field for use when
	   converting wireshark to new-style proto_tree. These fields
	   are merely strings on the GUI tree; they are not filterable */
	hf_text_only = proto_register_field_init(&hfi_text_only, -1);

	/* Register the pseudo-protocols used for exceptions. */
	register_show_exception();
	register_type_length_mismatch();
	register_number_string_decoding_error();
	register_string_errors();

	/* Have each built-in dissector register its protocols, fields,
	   dissector tables, and dissectors to be called through a
	   handle, and do whatever one-time initialization it needs to
	   do. */
	register_all_protocols(cb, client_data);

	/* Now call the registration routines for all epan plugins. */
	for (GSList *l = register_all_plugin_protocols_list; l != NULL; l = l->next) {
		((void (*)(register_cb, gpointer))l->data)(cb, client_data);
	}

#ifdef HAVE_PLUGINS
	/* Now call the registration routines for all dissector plugins. */
	if (cb)
		(*cb)(RA_PLUGIN_REGISTER, NULL, client_data);
	g_slist_foreach(dissector_plugins, call_plugin_register_protoinfo, NULL);
#endif

	/* Now call the "handoff registration" routines of all built-in
	   dissectors; those routines register the dissector in other
	   dissectors' handoff tables, and fetch any dissector handles
	   they need. */
	register_all_protocol_handoffs(cb, client_data);

	/* Now do the same with epan plugins. */
	for (GSList *l = register_all_plugin_handoffs_list; l != NULL; l = l->next) {
		((void (*)(register_cb, gpointer))l->data)(cb, client_data);
	}

#ifdef HAVE_PLUGINS
	/* Now do the same with dissector plugins. */
	if (cb)
		(*cb)(RA_PLUGIN_HANDOFF, NULL, client_data);
	g_slist_foreach(dissector_plugins, call_plugin_register_handoff, NULL);
#endif

	/* sort the protocols by protocol name */
	protocols = g_list_sort(protocols, proto_compare_name);

	/* We've assigned all the subtree type values; allocate the array
	   for them, and zero it out. */
	tree_is_expanded = g_new0(guint32, (num_tree_types/32)+1);
}