static u8 llcp_tlv_version(u8 *tlv)
{
	return llcp_tlv8(tlv, LLCP_TLV_VERSION);
}