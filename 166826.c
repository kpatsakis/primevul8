proto_tree_add_bitmask_value_with_flags(proto_tree *parent_tree, tvbuff_t *tvb, const guint offset,
		const int hf_hdr, const gint ett, int * const *fields, const guint64 value, const int flags)
{
	proto_item        *item = NULL;
	header_field_info *hf;
	int                len;

	PROTO_REGISTRAR_GET_NTH(hf_hdr,hf);
	DISSECTOR_ASSERT_FIELD_TYPE_IS_INTEGRAL(hf);
	/* the proto_tree_add_uint/_uint64() calls below
	   will fail if tvb==NULL and len!=0 */
	len = tvb ? ftype_length(hf->type) : 0;

	if (parent_tree) {
		if (len <= 4)
			item = proto_tree_add_uint(parent_tree, hf_hdr, tvb, offset, len, (guint32)value);
		else
			item = proto_tree_add_uint64(parent_tree, hf_hdr, tvb, offset, len, value);

		proto_item_add_bitmask_tree(item, tvb, offset, len, ett, fields,
		    flags, FALSE, FALSE, NULL, value);
	}

	return item;
}