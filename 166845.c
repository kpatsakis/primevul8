proto_tree_add_ts_23_038_7bits_packed_item(proto_tree *tree, const int hfindex, tvbuff_t *tvb,
	const guint bit_offset, const gint no_of_chars)
{
	proto_item	  *pi;
	header_field_info *hfinfo;
	gint		   byte_length;
	gint		   byte_offset;
	gchar		  *string;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hfinfo);

	DISSECTOR_ASSERT_FIELD_TYPE(hfinfo, FT_STRING);

	byte_length = (((no_of_chars + 1) * 7) + (bit_offset & 0x07)) >> 3;
	byte_offset = bit_offset >> 3;

	string = tvb_get_ts_23_038_7bits_string_packed(wmem_packet_scope(), tvb, bit_offset, no_of_chars);

	if (hfinfo->display == STR_UNICODE) {
		DISSECTOR_ASSERT(g_utf8_validate(string, -1, NULL));
	}

	pi = proto_tree_add_pi(tree, hfinfo, tvb, byte_offset, &byte_length);
	DISSECTOR_ASSERT(byte_length >= 0);
	proto_tree_set_string(PNODE_FINFO(pi), string);

	return pi;
}