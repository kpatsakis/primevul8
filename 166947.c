proto_tree_add_pi(proto_tree *tree, header_field_info *hfinfo, tvbuff_t *tvb, gint start,
		  gint *length)
{
	proto_item *pi;
	field_info *fi;
	gint		item_length;

	get_hfi_length(hfinfo, tvb, start, length, &item_length, ENC_NA);
	fi = new_field_info(tree, hfinfo, tvb, start, item_length);
	pi = proto_tree_add_node(tree, fi);

	return pi;
}