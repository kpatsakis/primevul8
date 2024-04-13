proto_tree_add_uint64_bits_format_value(proto_tree *tree, const int hfindex,
				      tvbuff_t *tvb, const guint bit_offset,
				      const gint no_of_bits, guint64 value,
				      const char *format, ...)
{
	va_list ap;
	gchar  *dst;
	header_field_info *hf_field;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hf_field);

	switch (hf_field->type) {
		case FT_UINT40:
		case FT_UINT48:
		case FT_UINT56:
		case FT_UINT64:
			break;

		default:
			REPORT_DISSECTOR_BUG("field %s is not of type FT_UINT40, FT_UINT48, FT_UINT56, or FT_UINT64",
			    hf_field->abbrev);
			return NULL;
			break;
	}

	CREATE_VALUE_STRING(dst, format, ap);

	return proto_tree_add_bits_format_value(tree, hfindex, tvb, bit_offset, no_of_bits, &value, dst);
}