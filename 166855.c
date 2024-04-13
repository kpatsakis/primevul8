proto_tree_add_item_ret_string_and_length(proto_tree *tree, int hfindex,
                                          tvbuff_t *tvb,
                                          const gint start, gint length,
                                          const guint encoding,
                                          wmem_allocator_t *scope,
                                          const guint8 **retval,
                                          gint *lenretval)
{
	proto_item *pi;
	header_field_info *hfinfo = proto_registrar_get_nth(hfindex);
	field_info	  *new_fi;
	const guint8	  *value;

	DISSECTOR_ASSERT_HINT(hfinfo != NULL, "Not passed hfi!");

	switch (hfinfo->type) {
	case FT_STRING:
		value = get_string_value(scope, tvb, start, length, lenretval, encoding);
		break;
	case FT_STRINGZ:
		value = get_stringz_value(scope, tree, tvb, start, length, lenretval, encoding);
		break;
	case FT_UINT_STRING:
		value = get_uint_string_value(scope, tree, tvb, start, length, lenretval, encoding);
		break;
	case FT_STRINGZPAD:
		value = get_stringzpad_value(scope, tvb, start, length, lenretval, encoding);
		break;
	case FT_STRINGZTRUNC:
		value = get_stringztrunc_value(scope, tvb, start, length, lenretval, encoding);
		break;
	default:
		REPORT_DISSECTOR_BUG("field %s is not of type FT_STRING, FT_STRINGZ, FT_UINT_STRING, FT_STRINGZPAD, or FT_STRINGZTRUNC",
		    hfinfo->abbrev);
	}

	if (retval)
		*retval = value;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfinfo->id, hfinfo);

	new_fi = new_field_info(tree, hfinfo, tvb, start, *lenretval);

	proto_tree_set_string(new_fi, value);

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

	default:
		g_assert_not_reached();
	}

	return pi;
}