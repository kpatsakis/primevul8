proto_tree_reset(proto_tree *tree)
{
	tree_data_t *tree_data = PTREE_DATA(tree);

	proto_tree_children_foreach(tree, proto_tree_free_node, NULL);

	/* free tree data */
	if (tree_data->interesting_hfids) {
		/* Free all the GPtrArray's in the interesting_hfids hash. */
		g_hash_table_foreach(tree_data->interesting_hfids,
			free_GPtrArray_value, NULL);

		/* And then remove all values. */
		g_hash_table_remove_all(tree_data->interesting_hfids);
	}

	/* Reset track of the number of children */
	tree_data->count = 0;

	PROTO_NODE_INIT(tree);
}