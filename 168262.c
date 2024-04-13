static u8 llcp_tlv8(u8 *tlv, u8 type)
{
	if (tlv[0] != type || tlv[1] != llcp_tlv_length[tlv[0]])
		return 0;

	return tlv[2];
}