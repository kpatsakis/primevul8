proto_tree_add_ipv4(proto_tree *tree, int hfindex, tvbuff_t *tvb, gint start,
		    gint length, ws_in4_addr value)
{
	proto_item	  *pi;
	header_field_info *hfinfo;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hfinfo);

	DISSECTOR_ASSERT_FIELD_TYPE(hfinfo, FT_IPv4);

	pi = proto_tree_add_pi(tree, hfinfo, tvb, start, &length);
	proto_tree_set_ipv4(PNODE_FINFO(pi), value);

	return pi;
}