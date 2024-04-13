ptvcursor_push_subtree(ptvcursor_t *ptvc, proto_item *it, gint ett_subtree)
{
	subtree_lvl *subtree;
	if (ptvc->pushed_tree_index >= ptvc->pushed_tree_max)
		ptvcursor_new_subtree_levels(ptvc);

	subtree = ptvc->pushed_tree + ptvc->pushed_tree_index;
	subtree->tree = ptvc->tree;
	subtree->it= NULL;
	ptvc->pushed_tree_index++;
	return ptvcursor_set_subtree(ptvc, it, ett_subtree);
}