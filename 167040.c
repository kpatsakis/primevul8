proto_tree_add_oid(proto_tree *tree, int hfindex, tvbuff_t *tvb, gint start,
		   gint length, const guint8* value_ptr)
{
	proto_item	  *pi;
	header_field_info *hfinfo;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hfinfo);

	DISSECTOR_ASSERT_FIELD_TYPE(hfinfo, FT_OID);

	pi = proto_tree_add_pi(tree, hfinfo, tvb, start, &length);
	proto_tree_set_oid(PNODE_FINFO(pi), value_ptr, length);

	return pi;
}