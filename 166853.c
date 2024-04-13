ptvcursor_pop_subtree(ptvcursor_t *ptvc)
{
	subtree_lvl *subtree;

	if (ptvc->pushed_tree_index <= 0)
		return;

	ptvc->pushed_tree_index--;
	subtree = ptvc->pushed_tree + ptvc->pushed_tree_index;
	if (subtree->it != NULL)
		proto_item_set_len(subtree->it, ptvcursor_current_offset(ptvc) - subtree->cursor_offset);

	ptvc->tree = subtree->tree;
}