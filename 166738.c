proto_tree_add_format_text(proto_tree *tree, tvbuff_t *tvb, gint start, gint length)
{
	proto_item	  *pi;
	header_field_info *hfinfo;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hf_text_only, hfinfo);

	pi = proto_tree_add_text_node(tree, tvb, start, length);

	TRY_TO_FAKE_THIS_REPR(pi);

	proto_item_set_text(pi, "%s", tvb_format_text(tvb, start, length));

	return pi;
}