proto_tree_add_bits_item(proto_tree *tree, const int hfindex, tvbuff_t *tvb,
			 const guint bit_offset, const gint no_of_bits,
			 const guint encoding)
{
	header_field_info *hfinfo;
	gint		   octet_length;
	gint		   octet_offset;

	PROTO_REGISTRAR_GET_NTH(hfindex, hfinfo);

	octet_length = (no_of_bits + 7) >> 3;
	octet_offset = bit_offset >> 3;
	test_length(hfinfo, tvb, octet_offset, octet_length, encoding);

	/* Yes, we try to fake this item again in proto_tree_add_bits_ret_val()
	 * but only after doing a bunch more work (which we can, in the common
	 * case, shortcut here).
	 */
	CHECK_FOR_NULL_TREE(tree);
	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hfinfo);

	return proto_tree_add_bits_ret_val(tree, hfindex, tvb, bit_offset, no_of_bits, NULL, encoding);
}