proto_tree_set_int64(field_info *fi, gint64 value)
{
	header_field_info *hfinfo;
	guint64		   integer;
	gint		   no_of_bits;

	hfinfo = fi->hfinfo;
	integer = value;

	if (hfinfo->bitmask) {
		/* Mask out irrelevant portions */
		integer &= hfinfo->bitmask;

		/* Shift bits */
		integer >>= hfinfo_bitshift(hfinfo);

		no_of_bits = ws_count_ones(hfinfo->bitmask);
		integer = ws_sign_ext64(integer, no_of_bits);

		FI_SET_FLAG(fi, FI_BITS_OFFSET(hfinfo_bitoffset(hfinfo)));
		FI_SET_FLAG(fi, FI_BITS_SIZE(hfinfo_mask_bitwidth(hfinfo)));
	}

	fvalue_set_sinteger64(&fi->value, integer);
}