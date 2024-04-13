proto_item_set_bits_offset_len(proto_item *ti, int bits_offset, int bits_len) {
	if (!ti) {
		return;
	}
	FI_SET_FLAG(PNODE_FINFO(ti), FI_BITS_OFFSET(bits_offset));
	FI_SET_FLAG(PNODE_FINFO(ti), FI_BITS_SIZE(bits_len));
}