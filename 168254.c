static u16 llcp_tlv_miux(u8 *tlv)
{
	return llcp_tlv16(tlv, LLCP_TLV_MIUX) & 0x7ff;
}