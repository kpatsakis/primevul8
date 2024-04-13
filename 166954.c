tree_expanded_set(int tree_type, gboolean value)
{
	g_assert(tree_type >= 0 && tree_type < num_tree_types);

	if (value)
		tree_is_expanded[tree_type >> 5] |= (1U << (tree_type & 31));
	else
		tree_is_expanded[tree_type >> 5] &= ~(1U << (tree_type & 31));
}