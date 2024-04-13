proto_tree_set_vines_tvb(field_info *fi, tvbuff_t *tvb, gint start)
{
	proto_tree_set_vines(fi, tvb_get_ptr(tvb, start, FT_VINES_ADDR_LEN));
}