proto_tree_add_bitmask(proto_tree *parent_tree, tvbuff_t *tvb,
		       const guint offset, const int hf_hdr,
		       const gint ett, int * const *fields,
		       const guint encoding)
{
	return proto_tree_add_bitmask_with_flags(parent_tree, tvb, offset, hf_hdr, ett, fields, encoding, BMT_NO_INT|BMT_NO_TFS);
}