proto_tree_set_bytes_tvb(field_info *fi, tvbuff_t *tvb, gint offset, gint length)
{
	proto_tree_set_bytes(fi, tvb_get_ptr(tvb, offset, length), length);
}