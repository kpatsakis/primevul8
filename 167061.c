proto_tree_add_float_bits_format_value(proto_tree *tree, const int hfindex,
				       tvbuff_t *tvb, const guint bit_offset,
				       const gint no_of_bits, float value,
				       const char *format, ...)
{
	va_list ap;
	gchar  *dst;
	header_field_info *hf_field;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hf_field);

	DISSECTOR_ASSERT_FIELD_TYPE(hf_field, FT_FLOAT);

	CREATE_VALUE_STRING(dst, format, ap);

	return proto_tree_add_bits_format_value(tree, hfindex, tvb, bit_offset, no_of_bits, &value, dst);
}