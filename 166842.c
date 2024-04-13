hfinfo_bitoffset(const header_field_info *hfinfo)
{
	if (!hfinfo->bitmask) {
		return 0;
	}

	/* ilog2 = first set bit, counting 0 as the last bit; we want 0
	 * as the first bit */
	return hfinfo_container_bitwidth(hfinfo) - 1 - ws_ilog2(hfinfo->bitmask);
}