proto_tree_add_item(proto_tree *tree, int hfindex, tvbuff_t *tvb,
		    const gint start, gint length, const guint encoding)
{
	register header_field_info *hfinfo;

	PROTO_REGISTRAR_GET_NTH(hfindex, hfinfo);
	return proto_tree_add_item_new(tree, hfinfo, tvb, start, length, encoding);
}