proto_tree_add_item_ret_uint(proto_tree *tree, int hfindex, tvbuff_t *tvb,
                             const gint start, gint length,
                             const guint encoding, guint32 *retval)
{
	header_field_info *hfinfo = proto_registrar_get_nth(hfindex);
	field_info	  *new_fi;
	guint32		   value;

	DISSECTOR_ASSERT_HINT(hfinfo != NULL, "Not passed hfi!");

	switch (hfinfo->type) {
	case FT_CHAR:
	case FT_UINT8:
	case FT_UINT16:
	case FT_UINT24:
	case FT_UINT32:
		break;
	default:
		REPORT_DISSECTOR_BUG("field %s is not of type FT_CHAR, FT_UINT8, FT_UINT16, FT_UINT24, or FT_UINT32",
		    hfinfo->abbrev);
	}

	/* length validation for native number encoding caught by get_uint_value() */
	/* length has to be -1 or > 0 regardless of encoding */
	if (length < -1 || length == 0)
		REPORT_DISSECTOR_BUG("Invalid length %d passed to proto_tree_add_item_ret_uint",
			length);

	if (encoding & ENC_STRING) {
		REPORT_DISSECTOR_BUG("wrong encoding");
	}
	/* I believe it's ok if this is called with a NULL tree */
	/* XXX - modify if we ever support EBCDIC FT_CHAR */
	if (encoding & (ENC_VARIANT_MASK)) {
		guint64 temp64;
		tvb_get_varint(tvb, start, length, &temp64, encoding);
		value = (guint32)temp64;
	} else {
		value = get_uint_value(tree, tvb, start, length, encoding);
	}

	if (retval) {
		*retval = value;
		if (hfinfo->bitmask) {
			/* Mask out irrelevant portions */
			*retval &= (guint32)(hfinfo->bitmask);
			/* Shift bits */
			*retval >>= hfinfo_bitshift(hfinfo);
		}
	}

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfinfo->id, hfinfo);

	new_fi = new_field_info(tree, hfinfo, tvb, start, length);

	proto_tree_set_uint(new_fi, value);

	new_fi->flags |= (encoding & ENC_LITTLE_ENDIAN) ? FI_LITTLE_ENDIAN : FI_BIG_ENDIAN;
	if (encoding & (ENC_VARINT_PROTOBUF|ENC_VARINT_ZIGZAG)) {
		new_fi->flags |= FI_VARINT;
	}
	return proto_tree_add_node(tree, new_fi);
}