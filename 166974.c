proto_tree_add_ipv6_format_value(proto_tree *tree, int hfindex, tvbuff_t *tvb,
				 gint start, gint length,
				 const ws_in6_addr *value_ptr,
				 const char *format, ...)
{
	proto_item	  *pi;
	va_list		   ap;

	pi = proto_tree_add_ipv6(tree, hfindex, tvb, start, length, value_ptr);
	if (pi != tree) {
		va_start(ap, format);
		proto_tree_set_representation_value(pi, format, ap);
		va_end(ap);
	}

	return pi;
}