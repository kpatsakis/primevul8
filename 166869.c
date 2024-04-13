proto_tree_add_item_ret_display_string(proto_tree *tree, int hfindex,
                                       tvbuff_t *tvb,
                                       const gint start, gint length,
                                       const guint encoding,
                                       wmem_allocator_t *scope,
                                       char **retval)
{
	return proto_tree_add_item_ret_display_string_and_length(tree, hfindex,
	    tvb, start, length, encoding, scope, retval, &length);
}