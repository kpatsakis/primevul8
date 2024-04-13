hfinfo_hex_digits(const header_field_info *hfinfo)
{
	int bitwidth;

	/* If we have a bitmask, hfinfo->type is the width of the container, so not
	 * appropriate to determine the number of hex digits for the field.
	 * So instead, we compute it from the bitmask.
	 */
	if (hfinfo->bitmask != 0) {
		bitwidth = hfinfo_mask_bitwidth(hfinfo);
	} else {
		bitwidth = hfinfo_type_bitwidth(hfinfo->type);
	}

	/* Divide by 4, rounding up, to get number of hex digits. */
	return (bitwidth + 3) / 4;
}