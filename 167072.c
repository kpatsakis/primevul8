proto_tree_add_item_ret_boolean(proto_tree *tree, int hfindex, tvbuff_t *tvb,
                                const gint start, gint length,
                                const guint encoding, gboolean *retval)
{
	header_field_info *hfinfo = proto_registrar_get_nth(hfindex);
	field_info	  *new_fi;
	guint64		   value, bitval;

	DISSECTOR_ASSERT_HINT(hfinfo != NULL, "Not passed hfi!");

	if (hfinfo->type != FT_BOOLEAN) {
		REPORT_DISSECTOR_BUG("field %s is not of type FT_BOOLEAN",
		    hfinfo->abbrev);
	}

	/* length validation for native number encoding caught by get_uint64_value() */
	/* length has to be -1 or > 0 regardless of encoding */
	if (length < -1 || length == 0)
		REPORT_DISSECTOR_BUG("Invalid length %d passed to proto_tree_add_item_ret_boolean",
			length);

	if (encoding & ENC_STRING) {
		REPORT_DISSECTOR_BUG("wrong encoding");
	}
	/* I believe it's ok if this is called with a NULL tree */
	value = get_uint64_value(tree, tvb, start, length, encoding);

	if (retval) {
		bitval = value;
		if (hfinfo->bitmask) {
			/* Mask out irrelevant portions */
			bitval &= hfinfo->bitmask;
		}
		*retval = (bitval != 0);
	}

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfinfo->id, hfinfo);

	new_fi = new_field_info(tree, hfinfo, tvb, start, length);

	proto_tree_set_boolean(new_fi, value);

	new_fi->flags |= (encoding & ENC_LITTLE_ENDIAN) ? FI_LITTLE_ENDIAN : FI_BIG_ENDIAN;

	return proto_tree_add_node(tree, new_fi);
}