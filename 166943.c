ptvcursor_add_subtree_item(ptvcursor_t *ptvc, proto_item *it, gint ett_subtree, gint length)
{
	ptvcursor_push_subtree(ptvc, it, ett_subtree);
	if (length == SUBTREE_UNDEFINED_LENGTH)
		ptvcursor_subtree_set_item(ptvc, it);
	return ptvcursor_tree(ptvc);
}