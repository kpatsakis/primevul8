proto_tree_add_item_ret_int(proto_tree *tree, int hfindex, tvbuff_t *tvb,
                            const gint start, gint length,
                            const guint encoding, gint32 *retval)
{
	header_field_info *hfinfo = proto_registrar_get_nth(hfindex);
	field_info	  *new_fi;
	gint32		   value;

	DISSECTOR_ASSERT_HINT(hfinfo != NULL, "Not passed hfi!");

	switch (hfinfo->type) {
	case FT_INT8:
	case FT_INT16:
	case FT_INT24:
	case FT_INT32:
		break;
	case FT_INT64:
		REPORT_DISSECTOR_BUG("64-bit signed integer field %s used with proto_tree_add_item_ret_int()",
		    hfinfo->abbrev);
	default:
		REPORT_DISSECTOR_BUG("Non-signed-integer field %s used with proto_tree_add_item_ret_int()",
		    hfinfo->abbrev);
	}

	/* length validation for native number encoding caught by get_uint_value() */
	/* length has to be -1 or > 0 regardless of encoding */
	if (length < -1 || length == 0)
		REPORT_DISSECTOR_BUG("Invalid length %d passed to proto_tree_add_item_ret_int",
			length);

	if (encoding & ENC_STRING) {
		REPORT_DISSECTOR_BUG("wrong encoding");
	}
	/* I believe it's ok if this is called with a NULL tree */
	value = get_int_value(tree, tvb, start, length, encoding);

	if (retval) {
		gint no_of_bits;
		*retval = value;
		if (hfinfo->bitmask) {
			/* Mask out irrelevant portions */
			*retval &= (guint32)(hfinfo->bitmask);
			/* Shift bits */
			*retval >>= hfinfo_bitshift(hfinfo);
		}
		no_of_bits = ws_count_ones(hfinfo->bitmask);
		*retval = ws_sign_ext32(*retval, no_of_bits);
	}

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfinfo->id, hfinfo);

	new_fi = new_field_info(tree, hfinfo, tvb, start, length);

	proto_tree_set_int(new_fi, value);

	new_fi->flags |= (encoding & ENC_LITTLE_ENDIAN) ? FI_LITTLE_ENDIAN : FI_BIG_ENDIAN;

	return proto_tree_add_node(tree, new_fi);
}