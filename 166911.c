proto_tree_add_item_ret_uint64(proto_tree *tree, int hfindex, tvbuff_t *tvb,
    const gint start, gint length, const guint encoding, guint64 *retval)
{
	header_field_info *hfinfo = proto_registrar_get_nth(hfindex);
	field_info	  *new_fi;
	guint64		   value;

	DISSECTOR_ASSERT_HINT(hfinfo != NULL, "Not passed hfi!");

	switch (hfinfo->type) {
	case FT_UINT40:
	case FT_UINT48:
	case FT_UINT56:
	case FT_UINT64:
		break;
	default:
		REPORT_DISSECTOR_BUG("field %s is not of type FT_UINT40, FT_UINT48, FT_UINT56, or FT_UINT64",
		    hfinfo->abbrev);
	}

	/* length validation for native number encoding caught by get_uint64_value() */
	/* length has to be -1 or > 0 regardless of encoding */
	if (length < -1 || length == 0)
		REPORT_DISSECTOR_BUG("Invalid length %d passed to proto_tree_add_item_ret_uint64",
			length);

	if (encoding & ENC_STRING) {
		REPORT_DISSECTOR_BUG("wrong encoding");
	}
	/* I believe it's ok if this is called with a NULL tree */
	if (encoding & (ENC_VARIANT_MASK)) {
		tvb_get_varint(tvb, start, length, &value, encoding);
	} else {
		value = get_uint64_value(tree, tvb, start, length, encoding);
	}

	if (retval) {
		*retval = value;
		if (hfinfo->bitmask) {
			/* Mask out irrelevant portions */
			*retval &= hfinfo->bitmask;
			/* Shift bits */
			*retval >>= hfinfo_bitshift(hfinfo);
		}
	}

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfinfo->id, hfinfo);

	new_fi = new_field_info(tree, hfinfo, tvb, start, length);

	proto_tree_set_uint64(new_fi, value);

	new_fi->flags |= (encoding & ENC_LITTLE_ENDIAN) ? FI_LITTLE_ENDIAN : FI_BIG_ENDIAN;
	if (encoding & (ENC_VARINT_PROTOBUF|ENC_VARINT_ZIGZAG)) {
		new_fi->flags |= FI_VARINT;
	}

	return proto_tree_add_node(tree, new_fi);
}