proto_tree_add_protocol_format(proto_tree *tree, int hfindex, tvbuff_t *tvb,
			       gint start, gint length, const char *format, ...)
{
	proto_item	  *pi;
	tvbuff_t	  *protocol_tvb;
	va_list		   ap;
	header_field_info *hfinfo;
	gchar* protocol_rep;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hfinfo);

	DISSECTOR_ASSERT_FIELD_TYPE(hfinfo, FT_PROTOCOL);

	/*
	 * This can throw an exception, so do it before we allocate anything.
	 */
	protocol_tvb = (start == 0 ? tvb : tvb_new_subset_length(tvb, start, length));

	pi = proto_tree_add_pi(tree, hfinfo, tvb, start, &length);

	va_start(ap, format);
	protocol_rep = g_strdup_vprintf(format, ap);
	proto_tree_set_protocol_tvb(PNODE_FINFO(pi), protocol_tvb, protocol_rep);
	g_free(protocol_rep);
	va_end(ap);

	TRY_TO_FAKE_THIS_REPR(pi);

	va_start(ap, format);
	proto_tree_set_representation(pi, format, ap);
	va_end(ap);

	return pi;
}