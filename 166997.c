tree_expanded(int tree_type)
{
	if (tree_type == -1) {
		return FALSE;
	}
	g_assert(tree_type >= 0 && tree_type < num_tree_types);
	return tree_is_expanded[tree_type >> 5] & (1U << (tree_type & 31));
}