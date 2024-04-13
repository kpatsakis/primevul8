ptvcursor_subtree_set_item(ptvcursor_t *ptvc, proto_item *it)
{
	subtree_lvl *subtree;

	DISSECTOR_ASSERT(ptvc->pushed_tree_index > 0);

	subtree                = ptvc->pushed_tree + ptvc->pushed_tree_index - 1;
	subtree->it            = it;
	subtree->cursor_offset = ptvcursor_current_offset(ptvc);
}