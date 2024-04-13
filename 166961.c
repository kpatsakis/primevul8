ptvcursor_add_ret_string(ptvcursor_t* ptvc, int hf, gint length, const guint encoding, wmem_allocator_t *scope, const guint8 **retval)
{
	header_field_info *hfinfo;
	field_info		*new_fi;
	const guint8	*value;
	gint			item_length;
	int				offset;

	offset = ptvc->offset;

	PROTO_REGISTRAR_GET_NTH(hf, hfinfo);

	switch (hfinfo->type) {
	case FT_STRING:
		value = get_string_value(scope, ptvc->tvb, offset, length, &item_length, encoding);
		break;
	case FT_STRINGZ:
		value = get_stringz_value(scope, ptvc->tree, ptvc->tvb, offset, length, &item_length, encoding);
		break;
	case FT_UINT_STRING:
		value = get_uint_string_value(scope, ptvc->tree, ptvc->tvb, offset, length, &item_length, encoding);
		break;
	case FT_STRINGZPAD:
		value = get_stringzpad_value(scope, ptvc->tvb, offset, length, &item_length, encoding);
		break;
	case FT_STRINGZTRUNC:
		value = get_stringztrunc_value(scope, ptvc->tvb, offset, length, &item_length, encoding);
		break;
	default:
		REPORT_DISSECTOR_BUG("field %s is not of type FT_STRING, FT_STRINGZ, FT_UINT_STRING, FT_STRINGZPAD, or FT_STRINGZTRUNC",
		    hfinfo->abbrev);
	}

	if (retval)
		*retval = value;

	ptvc->offset += item_length;

	CHECK_FOR_NULL_TREE(ptvc->tree);

	TRY_TO_FAKE_THIS_ITEM(ptvc->tree, hfinfo->id, hfinfo);

	new_fi = new_field_info(ptvc->tree, hfinfo, ptvc->tvb, offset, item_length);

	return proto_tree_new_item(new_fi, ptvc->tree, ptvc->tvb,
		offset, length, encoding);
}