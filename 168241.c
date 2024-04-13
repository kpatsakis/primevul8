u8 *nfc_llcp_build_tlv(u8 type, u8 *value, u8 value_length, u8 *tlv_length)
{
	u8 *tlv, length;

	pr_debug("type %d\n", type);

	if (type >= LLCP_TLV_MAX)
		return NULL;

	length = llcp_tlv_length[type];
	if (length == 0 && value_length == 0)
		return NULL;
	else if (length == 0)
		length = value_length;

	*tlv_length = 2 + length;
	tlv = kzalloc(2 + length, GFP_KERNEL);
	if (tlv == NULL)
		return tlv;

	tlv[0] = type;
	tlv[1] = length;
	memcpy(tlv + 2, value, length);

	return tlv;
}