ptvcursor_add_text_with_subtree(ptvcursor_t *ptvc, gint length,
				gint ett_subtree, const char *format, ...)
{
	proto_item        *pi;
	va_list            ap;
	header_field_info *hfinfo;
	proto_tree        *tree;

	tree = ptvcursor_tree(ptvc);

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hf_text_only, hfinfo);

	pi = proto_tree_add_text_node(tree, ptvcursor_tvbuff(ptvc),
				      ptvcursor_current_offset(ptvc), length);

	TRY_TO_FAKE_THIS_REPR(pi);

	va_start(ap, format);
	proto_tree_set_representation(pi, format, ap);
	va_end(ap);

	return ptvcursor_add_subtree_item(ptvc, pi, ett_subtree, length);
}