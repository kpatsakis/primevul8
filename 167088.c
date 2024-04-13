ptvcursor_new_subtree_levels(ptvcursor_t *ptvc)
{
	subtree_lvl *pushed_tree;

	DISSECTOR_ASSERT(ptvc->pushed_tree_max <= SUBTREE_MAX_LEVELS-SUBTREE_ONCE_ALLOCATION_NUMBER);
	ptvc->pushed_tree_max += SUBTREE_ONCE_ALLOCATION_NUMBER;

	pushed_tree = (subtree_lvl *)wmem_alloc(wmem_packet_scope(), sizeof(subtree_lvl) * ptvc->pushed_tree_max);
	DISSECTOR_ASSERT(pushed_tree != NULL);
	if (ptvc->pushed_tree)
		memcpy(pushed_tree, ptvc->pushed_tree, ptvc->pushed_tree_max - SUBTREE_ONCE_ALLOCATION_NUMBER);
	ptvc->pushed_tree = pushed_tree;
}