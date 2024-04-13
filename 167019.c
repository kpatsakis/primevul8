ptvcursor_add_ret_uint(ptvcursor_t *ptvc, int hfindex, gint length,
	      const guint encoding, guint32 *retval)
{
	field_info	  *new_fi;
	header_field_info *hfinfo;
	gint		   item_length;
	int		   offset;
	guint32		   value;

	offset = ptvc->offset;
	PROTO_REGISTRAR_GET_NTH(hfindex, hfinfo);

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

	get_hfi_length(hfinfo, ptvc->tvb, offset, &length, &item_length, encoding);
	test_length(hfinfo, ptvc->tvb, offset, item_length, encoding);

	/* I believe it's ok if this is called with a NULL tree */
	/* XXX - modify if we ever support EBCDIC FT_CHAR */
	value = get_uint_value(ptvc->tree, ptvc->tvb, offset, item_length, encoding);

	if (retval) {
		*retval = value;
		if (hfinfo->bitmask) {
			/* Mask out irrelevant portions */
			*retval &= (guint32)(hfinfo->bitmask);
			/* Shift bits */
			*retval >>= hfinfo_bitshift(hfinfo);
		}
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