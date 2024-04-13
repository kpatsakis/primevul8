proto_tree_add_item_ret_string(proto_tree *tree, int hfindex, tvbuff_t *tvb,
                               const gint start, gint length,
                               const guint encoding, wmem_allocator_t *scope,
                               const guint8 **retval)
{
	return proto_tree_add_item_ret_string_and_length(tree, hfindex,
	    tvb, start, length, encoding, scope, retval, &length);
}