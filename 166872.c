tree_data_add_maybe_interesting_field(tree_data_t *tree_data, field_info *fi)
{
	const header_field_info *hfinfo = fi->hfinfo;

	if (hfinfo->ref_type == HF_REF_TYPE_DIRECT) {
		GPtrArray *ptrs = NULL;

		if (tree_data->interesting_hfids == NULL) {
			/* Initialize the hash because we now know that it is needed */
			tree_data->interesting_hfids =
				g_hash_table_new(g_direct_hash, NULL /* g_direct_equal */);
		} else if (g_hash_table_size(tree_data->interesting_hfids)) {
			ptrs = (GPtrArray *)g_hash_table_lookup(tree_data->interesting_hfids,
					   GINT_TO_POINTER(hfinfo->id));
		}

		if (!ptrs) {
			/* First element triggers the creation of pointer array */
			ptrs = g_ptr_array_new();
			g_hash_table_insert(tree_data->interesting_hfids,
					    GINT_TO_POINTER(hfinfo->id), ptrs);
		}

		g_ptr_array_add(ptrs, fi);
	}
}