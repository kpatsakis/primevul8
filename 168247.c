static u16 llcp_tlv16(u8 *tlv, u8 type)
{
	if (tlv[0] != type || tlv[1] != llcp_tlv_length[tlv[0]])
		return 0;

	return be16_to_cpu(*((__be16 *)(tlv + 2)));
}