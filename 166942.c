proto_tree_set_ipv6_tvb(field_info *fi, tvbuff_t *tvb, gint start, gint length)
{
	proto_tree_set_ipv6(fi, tvb_get_ptr(tvb, start, length));
}