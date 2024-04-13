proto_tree_add_int_bits_format_value(proto_tree *tree, const int hfindex,
				     tvbuff_t *tvb, const guint bit_offset,
				     const gint no_of_bits, gint32 value,
				     const char *format, ...)
{
	va_list ap;
	gchar  *dst;
	header_field_info *hf_field;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hf_field);

	switch (hf_field->type) {
		case FT_INT8:
		case FT_INT16:
		case FT_INT24:
		case FT_INT32:
			break;

		default:
			REPORT_DISSECTOR_BUG("field %s is not of type FT_INT8, FT_INT16, FT_INT24, or FT_INT32",
			    hf_field->abbrev);
			return NULL;
			break;
	}

	CREATE_VALUE_STRING(dst, format, ap);

	return proto_tree_add_bits_format_value(tree, hfindex, tvb, bit_offset, no_of_bits, &value, dst);
}