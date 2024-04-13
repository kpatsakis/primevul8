int nfc_llcp_parse_gb_tlv(struct nfc_llcp_local *local,
			  u8 *tlv_array, u16 tlv_array_len)
{
	u8 *tlv = tlv_array, type, length, offset = 0;

	pr_debug("TLV array length %d\n", tlv_array_len);

	if (local == NULL)
		return -ENODEV;

	while (offset < tlv_array_len) {
		type = tlv[0];
		length = tlv[1];

		pr_debug("type 0x%x length %d\n", type, length);

		switch (type) {
		case LLCP_TLV_VERSION:
			local->remote_version = llcp_tlv_version(tlv);
			break;
		case LLCP_TLV_MIUX:
			local->remote_miu = llcp_tlv_miux(tlv) + 128;
			break;
		case LLCP_TLV_WKS:
			local->remote_wks = llcp_tlv_wks(tlv);
			break;
		case LLCP_TLV_LTO:
			local->remote_lto = llcp_tlv_lto(tlv) * 10;
			break;
		case LLCP_TLV_OPT:
			local->remote_opt = llcp_tlv_opt(tlv);
			break;
		default:
			pr_err("Invalid gt tlv value 0x%x\n", type);
			break;
		}

		offset += length + 2;
		tlv += length + 2;
	}

	pr_debug("version 0x%x miu %d lto %d opt 0x%x wks 0x%x\n",
		 local->remote_version, local->remote_miu,
		 local->remote_lto, local->remote_opt,
		 local->remote_wks);

	return 0;
}