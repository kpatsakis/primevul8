proto_tree_set_guid_tvb(field_info *fi, tvbuff_t *tvb, gint start,
			const guint encoding)
{
	e_guid_t guid;

	tvb_get_guid(tvb, start, &guid, encoding);
	proto_tree_set_guid(fi, &guid);
}