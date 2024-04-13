proto_tree_add_format_wsp_text(proto_tree *tree, tvbuff_t *tvb, gint start, gint length)
{
	proto_item	  *pi;
	header_field_info *hfinfo;
	gchar		  *str;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hf_text_only, hfinfo);

	pi = proto_tree_add_text_node(tree, tvb, start, length);

	TRY_TO_FAKE_THIS_REPR(pi);

	str = tvb_format_text_wsp(NULL, tvb, start, length);
	proto_item_set_text(pi, "%s", str);
	wmem_free(NULL, str);

	return pi;
}