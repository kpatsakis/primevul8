proto_tree_add_string(proto_tree *tree, int hfindex, tvbuff_t *tvb, gint start,
		      gint length, const char* value)
{
	proto_item	  *pi;
	header_field_info *hfinfo;
	gint		  item_length;

	PROTO_REGISTRAR_GET_NTH(hfindex, hfinfo);
	get_hfi_length(hfinfo, tvb, start, &length, &item_length, ENC_NA);
	/*
	 * Special case - if the length is 0, skip the test, so that
	 * we can have an empty string right after the end of the
	 * packet.  (This handles URL-encoded forms where the last field
	 * has no value so the form ends right after the =.)
	 */
	if (item_length != 0)
		test_length(hfinfo, tvb, start, item_length, ENC_NA);

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hfinfo);

	DISSECTOR_ASSERT_FIELD_TYPE_IS_STRING(hfinfo);

	pi = proto_tree_add_pi(tree, hfinfo, tvb, start, &length);
	DISSECTOR_ASSERT(length >= 0);
	proto_tree_set_string(PNODE_FINFO(pi), value);

	return pi;
}