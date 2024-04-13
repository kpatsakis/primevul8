static u8 llcp_tlv_rw(u8 *tlv)
{
	return llcp_tlv8(tlv, LLCP_TLV_RW) & 0xf;
}