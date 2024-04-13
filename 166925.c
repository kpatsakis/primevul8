proto_tree_set_oid_tvb(field_info *fi, tvbuff_t *tvb, gint start, gint length)
{
	proto_tree_set_oid(fi, tvb_get_ptr(tvb, start, length), length);
}