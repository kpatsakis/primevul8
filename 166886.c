proto_tree_set_ax25_tvb(field_info *fi, tvbuff_t *tvb, gint start)
{
	proto_tree_set_ax25(fi, tvb_get_ptr(tvb, start, 7));
}