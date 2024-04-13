hfinfo_bitshift(const header_field_info *hfinfo)
{
	return ws_ctz(hfinfo->bitmask);
}