proto_tree_add_bytes_format_value(proto_tree *tree, int hfindex, tvbuff_t *tvb,
				  gint start, gint length,
				  const guint8 *start_ptr,
				  const char *format, ...)
{
	proto_item	  *pi;
	va_list		   ap;

	if (start_ptr == NULL)
		start_ptr = tvb_get_ptr(tvb, start, length);

	pi = proto_tree_add_bytes(tree, hfindex, tvb, start, length, start_ptr);

	TRY_TO_FAKE_THIS_REPR_NESTED(pi);

	va_start(ap, format);
	proto_tree_set_representation_value(pi, format, ap);
	va_end(ap);

	return pi;
}