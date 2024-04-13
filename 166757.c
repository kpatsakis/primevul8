ptvcursor_set_subtree(ptvcursor_t *ptvc, proto_item *it, gint ett_subtree)
{
	ptvc->tree = proto_item_add_subtree(it, ett_subtree);
	return ptvc->tree;
}