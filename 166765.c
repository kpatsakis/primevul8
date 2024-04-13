proto_tree_add_item_ret_ipv4(proto_tree *tree, int hfindex, tvbuff_t *tvb,
                             const gint start, gint length,
                             const guint encoding, ws_in4_addr *retval)
{
	header_field_info *hfinfo = proto_registrar_get_nth(hfindex);
	field_info	  *new_fi;
	ws_in4_addr	   value;

	DISSECTOR_ASSERT_HINT(hfinfo != NULL, "Not passed hfi!");

	switch (hfinfo->type) {
	case FT_IPv4:
		break;
	default:
		REPORT_DISSECTOR_BUG("field %s is not of type FT_IPv4",
		    hfinfo->abbrev);
	}

	if (length != FT_IPv4_LEN)
		REPORT_DISSECTOR_BUG("Invalid length %d passed to proto_tree_add_item_ret_ipv4",
			length);

	if (encoding & (ENC_STRING | ENC_VARIANT_MASK | ENC_VARINT_PROTOBUF | ENC_VARINT_ZIGZAG)) {
		REPORT_DISSECTOR_BUG("wrong encoding");
	}

	/*
	 * NOTE: to support code written when proto_tree_add_item() took
	 * a gboolean as its last argument, with FALSE meaning "big-endian"
	 * and TRUE meaning "little-endian", we treat any non-zero value
	 * of "encoding" as meaning "little-endian".
	 */
	value = tvb_get_ipv4(tvb, start);
	if (encoding)
		value = GUINT32_SWAP_LE_BE(value);

	if (retval) {
		*retval = value;
	}

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfinfo->id, hfinfo);

	new_fi = new_field_info(tree, hfinfo, tvb, start, length);

	proto_tree_set_ipv4(new_fi, value);

	new_fi->flags |= encoding ? FI_LITTLE_ENDIAN : FI_BIG_ENDIAN;
	return proto_tree_add_node(tree, new_fi);
}