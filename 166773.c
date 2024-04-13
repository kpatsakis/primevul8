proto_tree_add_bits_ret_val(proto_tree *tree, const int hfindex, tvbuff_t *tvb,
			    const guint bit_offset, const gint no_of_bits,
			    guint64 *return_value, const guint encoding)
{
	proto_item *item;

	if ((item = _proto_tree_add_bits_ret_val(tree, hfindex, tvb,
						 bit_offset, no_of_bits,
						 return_value, encoding))) {
		FI_SET_FLAG(PNODE_FINFO(item), FI_BITS_OFFSET(bit_offset));
		FI_SET_FLAG(PNODE_FINFO(item), FI_BITS_SIZE(no_of_bits));
	}
	return item;
}