proto_tree_add_bitmask_list(proto_tree *tree, tvbuff_t *tvb, const guint offset,
								const int len, int * const *fields, const guint encoding)
{
	guint64 value;

	if (tree) {
		value = get_uint64_value(tree, tvb, offset, len, encoding);
		proto_item_add_bitmask_tree(NULL, tvb, offset, len, -1, fields,
		    BMT_NO_APPEND, FALSE, TRUE, tree, value);
	}
}