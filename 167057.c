proto_tree_add_debug_text(proto_tree *tree, const char *format, ...)
{
	proto_item *pi;
	va_list	    ap;

	pi = proto_tree_add_text_node(tree, NULL, 0, 0);

	if (pi) {
		va_start(ap, format);
		proto_tree_set_representation(pi, format, ap);
		va_end(ap);
	}
	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
	ws_debug_printf("\n");

	return pi;
}