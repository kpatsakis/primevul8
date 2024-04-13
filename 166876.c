proto_tree_set_uint(field_info *fi, guint32 value)
{
	header_field_info *hfinfo;
	guint32		   integer;

	hfinfo = fi->hfinfo;
	integer = value;

	if (hfinfo->bitmask) {
		/* Mask out irrelevant portions */
		integer &= (guint32)(hfinfo->bitmask);

		/* Shift bits */
		integer >>= hfinfo_bitshift(hfinfo);

		FI_SET_FLAG(fi, FI_BITS_OFFSET(hfinfo_bitoffset(hfinfo)));
		FI_SET_FLAG(fi, FI_BITS_SIZE(hfinfo_mask_bitwidth(hfinfo)));
	}

	fvalue_set_uinteger(&fi->value, integer);
}