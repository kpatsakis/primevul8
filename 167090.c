ptvcursor_add_ret_int(ptvcursor_t *ptvc, int hfindex, gint length,
	      const guint encoding, gint32 *retval)
{
	field_info	  *new_fi;
	header_field_info *hfinfo;
	gint		   item_length;
	int		   offset;
	guint32		   value;

	offset = ptvc->offset;
	PROTO_REGISTRAR_GET_NTH(hfindex, hfinfo);

	switch (hfinfo->type) {
	case FT_INT8:
	case FT_INT16:
	case FT_INT24:
	case FT_INT32:
		break;
	default:
		REPORT_DISSECTOR_BUG("field %s is not of type FT_INT8, FT_INT16, FT_INT24, or FT_INT32",
		    hfinfo->abbrev);
	}

	get_hfi_length(hfinfo, ptvc->tvb, offset, &length, &item_length, encoding);
	test_length(hfinfo, ptvc->tvb, offset, item_length, encoding);

	/* I believe it's ok if this is called with a NULL tree */
	/* XXX - modify if we ever support EBCDIC FT_CHAR */
	value = get_int_value(ptvc->tree, ptvc->tvb, offset, item_length, encoding);

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

	ptvc->offset += get_full_length(hfinfo, ptvc->tvb, offset, length,
	    item_length, encoding);

	CHECK_FOR_NULL_TREE(ptvc->tree);

	/* Coast clear. Try and fake it */
	TRY_TO_FAKE_THIS_ITEM(ptvc->tree, hfindex, hfinfo);

	new_fi = new_field_info(ptvc->tree, hfinfo, ptvc->tvb, offset, item_length);

	return proto_tree_new_item(new_fi, ptvc->tree, ptvc->tvb,
		offset, length, encoding);
}