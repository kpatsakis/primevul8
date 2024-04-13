proto_tree_set_eui64_tvb(field_info *fi, tvbuff_t *tvb, gint start, const guint encoding)
{
	if (encoding)
	{
		proto_tree_set_eui64(fi, tvb_get_letoh64(tvb, start));
	} else {
		proto_tree_set_eui64(fi, tvb_get_ntoh64(tvb, start));
	}
}