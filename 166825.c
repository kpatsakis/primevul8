proto_tree_free(proto_tree *tree)
{
	tree_data_t *tree_data = PTREE_DATA(tree);

	proto_tree_children_foreach(tree, proto_tree_free_node, NULL);

	/* free tree data */
	if (tree_data->interesting_hfids) {
		/* Free all the GPtrArray's in the interesting_hfids hash. */
		g_hash_table_foreach(tree_data->interesting_hfids,
			free_GPtrArray_value, NULL);

		/* And then destroy the hash. */
		g_hash_table_destroy(tree_data->interesting_hfids);
	}

	g_slice_free(tree_data_t, tree_data);

	g_slice_free(proto_tree, tree);
}