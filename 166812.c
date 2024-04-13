proto_tree_add_bytes_with_length(proto_tree *tree, int hfindex, tvbuff_t *tvb, gint start,
             gint tvbuff_length, const guint8 *start_ptr, gint ptr_length)
{
	proto_item    *pi;
	header_field_info *hfinfo;
	gint          item_length;

	PROTO_REGISTRAR_GET_NTH(hfindex, hfinfo);
	get_hfi_length(hfinfo, tvb, start, &tvbuff_length, &item_length, ENC_NA);
	test_length(hfinfo, tvb, start, item_length, ENC_NA);

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hfinfo);

	DISSECTOR_ASSERT_FIELD_TYPE(hfinfo, FT_BYTES);

	pi = proto_tree_add_pi(tree, hfinfo, tvb, start, &tvbuff_length);
	proto_tree_set_bytes(PNODE_FINFO(pi), start_ptr, ptr_length);

	return pi;
}