proto_tree_add_ipxnet_format(proto_tree *tree, int hfindex, tvbuff_t *tvb,
			     gint start, gint length, guint32 value,
			     const char *format, ...)
{
	proto_item	  *pi;
	va_list		   ap;

	pi = proto_tree_add_ipxnet(tree, hfindex, tvb, start, length, value);
	if (pi != tree) {
		TRY_TO_FAKE_THIS_REPR(pi);

		va_start(ap, format);
		proto_tree_set_representation(pi, format, ap);
		va_end(ap);
	}

	return pi;
}