proto_tree_add_item_ret_display_string_and_length(proto_tree *tree, int hfindex,
                                                  tvbuff_t *tvb,
                                                  const gint start, gint length,
                                                  const guint encoding,
                                                  wmem_allocator_t *scope,
                                                  char **retval,
                                                  gint *lenretval)
{
	proto_item *pi;
	header_field_info *hfinfo = proto_registrar_get_nth(hfindex);
	field_info	  *new_fi;
	const guint8	  *value;
	guint32		   n = 0;

	DISSECTOR_ASSERT_HINT(hfinfo != NULL, "Not passed hfi!");

	switch (hfinfo->type) {
	case FT_STRING:
		value = get_string_value(scope, tvb, start, length, lenretval, encoding);
		*retval = hfinfo_format_text(scope, hfinfo, value);
		break;
	case FT_STRINGZ:
		value = get_stringz_value(scope, tree, tvb, start, length, lenretval, encoding);
		*retval = hfinfo_format_text(scope, hfinfo, value);
		break;
	case FT_UINT_STRING:
		value = get_uint_string_value(scope, tree, tvb, start, length, lenretval, encoding);
		*retval = hfinfo_format_text(scope, hfinfo, value);
		break;
	case FT_STRINGZPAD:
		value = get_stringzpad_value(scope, tvb, start, length, lenretval, encoding);
		*retval = hfinfo_format_text(scope, hfinfo, value);
		break;
	case FT_STRINGZTRUNC:
		value = get_stringztrunc_value(scope, tvb, start, length, lenretval, encoding);
		*retval = hfinfo_format_text(scope, hfinfo, value);
		break;
	case FT_BYTES:
		value = tvb_get_ptr(tvb, start, length);
		*retval = hfinfo_format_bytes(scope, hfinfo, value, length);
		*lenretval = length;
		break;
	case FT_UINT_BYTES:
		n = get_uint_value(tree, tvb, start, length, encoding);
		value = tvb_get_ptr(tvb, start + length, n);
		*retval = hfinfo_format_bytes(scope, hfinfo, value, n);
		*lenretval = length + n;
		break;
	default:
		REPORT_DISSECTOR_BUG("field %s is not of type FT_STRING, FT_STRINGZ, FT_UINT_STRING, FT_STRINGZPAD, FT_STRINGZTRUNC, FT_BYTES, or FT_UINT_BYTES",
		    hfinfo->abbrev);
	}

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfinfo->id, hfinfo);

	new_fi = new_field_info(tree, hfinfo, tvb, start, *lenretval);

	switch (hfinfo->type) {

	case FT_STRING:
	case FT_STRINGZ:
	case FT_UINT_STRING:
	case FT_STRINGZPAD:
	case FT_STRINGZTRUNC:
		proto_tree_set_string(new_fi, value);
		break;

	case FT_BYTES:
		proto_tree_set_bytes(new_fi, value, length);
		break;

	case FT_UINT_BYTES:
		proto_tree_set_bytes(new_fi, value, n);
		break;
	default:
		g_assert_not_reached();
	}

	new_fi->flags |= (encoding & ENC_LITTLE_ENDIAN) ? FI_LITTLE_ENDIAN : FI_BIG_ENDIAN;

	pi = proto_tree_add_node(tree, new_fi);

	switch (hfinfo->type) {

	case FT_STRINGZ:
	case FT_STRINGZPAD:
	case FT_STRINGZTRUNC:
	case FT_UINT_STRING:
		break;

	case FT_STRING:
		detect_trailing_stray_characters(encoding, value, length, pi);
		break;

	case FT_BYTES:
	case FT_UINT_BYTES:
		break;

	default:
		g_assert_not_reached();
	}

	return pi;
}