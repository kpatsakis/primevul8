proto_tree_add_bitmask_list_value(proto_tree *tree, tvbuff_t *tvb, const guint offset,
								const int len, int * const *fields, const guint64 value)
{
	if (tree) {
		proto_item_add_bitmask_tree(NULL, tvb, offset, len, -1, fields,
		    BMT_NO_APPEND, FALSE, TRUE, tree, value);
	}
}