proto_tree_add_none_format(proto_tree *tree, const int hfindex, tvbuff_t *tvb,
			   const gint start, gint length, const char *format,
			   ...)
{
	proto_item	  *pi;
	va_list		   ap;
	header_field_info *hfinfo;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hfinfo);

	DISSECTOR_ASSERT_FIELD_TYPE(hfinfo, FT_NONE);

	pi = proto_tree_add_pi(tree, hfinfo, tvb, start, &length);

	TRY_TO_FAKE_THIS_REPR(pi);

	va_start(ap, format);
	proto_tree_set_representation(pi, format, ap);
	va_end(ap);

	/* no value to set for FT_NONE */
	return pi;
}