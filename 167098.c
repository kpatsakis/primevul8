proto_tree_add_text_internal(proto_tree *tree, tvbuff_t *tvb, gint start, gint length,
		    const char *format, ...)
{
	proto_item	  *pi;
	va_list		   ap;
	header_field_info *hfinfo;

	if (length == -1) {
		length = tvb_captured_length(tvb) ? tvb_ensure_captured_length_remaining(tvb, start) : 0;
	} else {
		tvb_ensure_bytes_exist(tvb, start, length);
	}

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hf_text_only, hfinfo);

	pi = proto_tree_add_text_node(tree, tvb, start, length);

	TRY_TO_FAKE_THIS_REPR(pi);

	va_start(ap, format);
	proto_tree_set_representation(pi, format, ap);
	va_end(ap);

	return pi;
}