proto_tree_add_time(proto_tree *tree, int hfindex, tvbuff_t *tvb, gint start,
		    gint length, const nstime_t *value_ptr)
{
	proto_item	  *pi;
	header_field_info *hfinfo;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hfinfo);

	DISSECTOR_ASSERT_FIELD_TYPE_IS_TIME(hfinfo);

	pi = proto_tree_add_pi(tree, hfinfo, tvb, start, &length);
	proto_tree_set_time(PNODE_FINFO(pi), value_ptr);

	return pi;
}