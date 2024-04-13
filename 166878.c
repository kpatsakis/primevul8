proto_tree_add_ipv4_format_value(proto_tree *tree, int hfindex, tvbuff_t *tvb,
				 gint start, gint length, ws_in4_addr value,
				 const char *format, ...)
{
	proto_item	  *pi;
	va_list		   ap;

	pi = proto_tree_add_ipv4(tree, hfindex, tvb, start, length, value);
	if (pi != tree) {
		va_start(ap, format);
		proto_tree_set_representation_value(pi, format, ap);
		va_end(ap);
	}

	return pi;
}