proto_tree_add_bitmask_with_flags_ret_uint64(proto_tree *parent_tree, tvbuff_t *tvb, const guint offset,
		const int hf_hdr, const gint ett, int * const *fields, const guint encoding, const int flags,
		guint64 *retval)
{
	proto_item        *item = NULL;
	header_field_info *hf;
	int                len;
	guint64            value;

	PROTO_REGISTRAR_GET_NTH(hf_hdr,hf);
	DISSECTOR_ASSERT_FIELD_TYPE_IS_INTEGRAL(hf);
	len = ftype_length(hf->type);
	value = get_uint64_value(parent_tree, tvb, offset, len, encoding);

	if (parent_tree) {
		item = proto_tree_add_item(parent_tree, hf_hdr, tvb, offset, len, encoding);
		proto_item_add_bitmask_tree(item, tvb, offset, len, ett, fields,
		    flags, FALSE, FALSE, NULL, value);
	}

	*retval = value;
	if (hf->bitmask) {
		/* Mask out irrelevant portions */
		*retval &= hf->bitmask;
		/* Shift bits */
		*retval >>= hfinfo_bitshift(hf);
	}

	return item;
}