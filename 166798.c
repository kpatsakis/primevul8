proto_tree_add_boolean_bits_format_value64(proto_tree *tree, const int hfindex,
					 tvbuff_t *tvb, const guint bit_offset,
					 const gint no_of_bits, guint64 value,
					 const char *format, ...)
{
	va_list ap;
	gchar  *dst;
	header_field_info *hf_field;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hf_field);

	DISSECTOR_ASSERT_FIELD_TYPE(hf_field, FT_BOOLEAN);

	CREATE_VALUE_STRING(dst, format, ap);

	return proto_tree_add_bits_format_value(tree, hfindex, tvb, bit_offset, no_of_bits, &value, dst);
}