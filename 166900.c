ptvcursor_add_with_subtree(ptvcursor_t *ptvc, int hfindex, gint length,
			   const guint encoding, gint ett_subtree)
{
	proto_item *it;

	it = ptvcursor_add_no_advance(ptvc, hfindex, length, encoding);
	return ptvcursor_add_subtree_item(ptvc, it, ett_subtree, length);
}