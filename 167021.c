ptvcursor_add_ret_boolean(ptvcursor_t* ptvc, int hfindex, gint length, const guint encoding, gboolean *retval)
{
	header_field_info *hfinfo;
	field_info		*new_fi;
	gint			item_length;
	int				offset;
	guint64			value, bitval;

	offset = ptvc->offset;
	PROTO_REGISTRAR_GET_NTH(hfindex, hfinfo);

	if (hfinfo->type != FT_BOOLEAN) {
		REPORT_DISSECTOR_BUG("field %s is not of type FT_BOOLEAN",
		    hfinfo->abbrev);
	}

	/* length validation for native number encoding caught by get_uint64_value() */
	/* length has to be -1 or > 0 regardless of encoding */
	if (length < -1 || length == 0)
		REPORT_DISSECTOR_BUG("Invalid length %d passed to ptvcursor_add_ret_boolean",
			length);

	if (encoding & ENC_STRING) {
		REPORT_DISSECTOR_BUG("wrong encoding");
	}

	get_hfi_length(hfinfo, ptvc->tvb, offset, &length, &item_length, encoding);
	test_length(hfinfo, ptvc->tvb, offset, item_length, encoding);

	/* I believe it's ok if this is called with a NULL tree */
	value = get_uint64_value(ptvc->tree, ptvc->tvb, offset, length, encoding);

	if (retval) {
		bitval = value;
		if (hfinfo->bitmask) {
			/* Mask out irrelevant portions */
			bitval &= hfinfo->bitmask;
		}
		*retval = (bitval != 0);
	}

	ptvc->offset += get_full_length(hfinfo, ptvc->tvb, offset, length,
	    item_length, encoding);

	CHECK_FOR_NULL_TREE(ptvc->tree);

	TRY_TO_FAKE_THIS_ITEM(ptvc->tree, hfinfo->id, hfinfo);

	new_fi = new_field_info(ptvc->tree, hfinfo, ptvc->tvb, offset, item_length);

	return proto_tree_new_item(new_fi, ptvc->tree, ptvc->tvb,
		offset, length, encoding);
}