proto_tree_add_item_ret_length(proto_tree *tree, int hfindex, tvbuff_t *tvb,
			       const gint start, gint length,
			       const guint encoding, gint *lenretval)
{
	register header_field_info *hfinfo;

	PROTO_REGISTRAR_GET_NTH(hfindex, hfinfo);
	return proto_tree_add_item_new_ret_length(tree, hfinfo, tvb, start, length, encoding, lenretval);
}