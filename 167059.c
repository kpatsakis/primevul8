proto_tree_add_subtree_format(proto_tree *tree, tvbuff_t *tvb, gint start, gint length, gint idx, proto_item **tree_item, const char *format, ...)
{
	proto_tree *pt;
	proto_item *pi;
	va_list	    ap;

	va_start(ap, format);
	pi = proto_tree_add_text_valist_internal(tree, tvb, start, length, format, ap);
	va_end(ap);

	if (tree_item != NULL)
		*tree_item = pi;

	pt = proto_item_add_subtree(pi, idx);

	return pt;
}