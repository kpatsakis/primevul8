static int mwifiex_uap_custom_ie_prepare(u8 *tlv, void *cmd_buf, u16 *ie_size)
{
	struct mwifiex_ie_list *ap_ie = cmd_buf;
	struct mwifiex_ie_types_header *tlv_ie = (void *)tlv;

	if (!ap_ie || !ap_ie->len)
		return -1;

	*ie_size += le16_to_cpu(ap_ie->len) +
			sizeof(struct mwifiex_ie_types_header);

	tlv_ie->type = cpu_to_le16(TLV_TYPE_MGMT_IE);
	tlv_ie->len = ap_ie->len;
	tlv += sizeof(struct mwifiex_ie_types_header);

	memcpy(tlv, ap_ie->ie_list, le16_to_cpu(ap_ie->len));

	return 0;
}