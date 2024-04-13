int nfc_llcp_parse_connection_tlv(struct nfc_llcp_sock *sock,
				  u8 *tlv_array, u16 tlv_array_len)
{
	u8 *tlv = tlv_array, type, length, offset = 0;

	pr_debug("TLV array length %d\n", tlv_array_len);

	if (sock == NULL)
		return -ENOTCONN;

	while (offset < tlv_array_len) {
		type = tlv[0];
		length = tlv[1];

		pr_debug("type 0x%x length %d\n", type, length);

		switch (type) {
		case LLCP_TLV_MIUX:
			sock->remote_miu = llcp_tlv_miux(tlv) + 128;
			break;
		case LLCP_TLV_RW:
			sock->remote_rw = llcp_tlv_rw(tlv);
			break;
		case LLCP_TLV_SN:
			break;
		default:
			pr_err("Invalid gt tlv value 0x%x\n", type);
			break;
		}

		offset += length + 2;
		tlv += length + 2;
	}

	pr_debug("sock %p rw %d miu %d\n", sock,
		 sock->remote_rw, sock->remote_miu);

	return 0;
}