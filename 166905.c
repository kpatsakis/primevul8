proto_register_subtree_array(gint * const *indices, const int num_indices)
{
	int	i;
	gint	*const *ptr = indices;

	/*
	 * If we've already allocated the array of tree types, expand
	 * it; this lets plugins such as mate add tree types after
	 * the initial startup.  (If we haven't already allocated it,
	 * we don't allocate it; on the first pass, we just assign
	 * ett values and keep track of how many we've assigned, and
	 * when we're finished registering all dissectors we allocate
	 * the array, so that we do only one allocation rather than
	 * wasting CPU time and memory by growing the array for each
	 * dissector that registers ett values.)
	 */
	if (tree_is_expanded != NULL) {
		tree_is_expanded = (guint32 *)g_realloc(tree_is_expanded, (1+((num_tree_types + num_indices)/32)) * sizeof(guint32));

		/* set new items to 0 */
		/* XXX, slow!!! optimize when needed (align 'i' to 32, and set rest of guint32 to 0) */
		for (i = num_tree_types; i < num_tree_types + num_indices; i++)
			tree_is_expanded[i >> 5] &= ~(1U << (i & 31));
	}

	/*
	 * Assign "num_indices" subtree numbers starting at "num_tree_types",
	 * returning the indices through the pointers in the array whose
	 * first element is pointed to by "indices", and update
	 * "num_tree_types" appropriately.
	 */
	for (i = 0; i < num_indices; i++, ptr++, num_tree_types++) {
		if (**ptr != -1) {
			/* g_error will terminate the program */
			g_error("register_subtree_array: subtree item type (ett_...) not -1 !"
				" This is a development error:"
				" Either the subtree item type has already been assigned or"
				" was not initialized to -1.");
		}
		**ptr = num_tree_types;
	}
}