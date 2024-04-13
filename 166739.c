proto_find_field_from_offset(proto_tree *tree, guint offset, tvbuff_t *tvb)
{
	offset_search_t	offsearch;

	offsearch.offset = offset;
	offsearch.finfo  = NULL;
	offsearch.tvb    = tvb;

	proto_tree_traverse_pre_order(tree, check_for_offset, &offsearch);

	return offsearch.finfo;
}