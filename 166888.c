proto_tree_add_bitmask_value(proto_tree *parent_tree, tvbuff_t *tvb, const guint offset,
		const int hf_hdr, const gint ett, int * const *fields, const guint64 value)
{
	return proto_tree_add_bitmask_value_with_flags(parent_tree, tvb, offset,
						hf_hdr, ett, fields, value, BMT_NO_INT|BMT_NO_TFS);
}