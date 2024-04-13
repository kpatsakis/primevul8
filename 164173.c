mwifiex_uap_bss_wpa(u8 **tlv_buf, void *cmd_buf, u16 *param_size)
{
	struct host_cmd_tlv_pwk_cipher *pwk_cipher;
	struct host_cmd_tlv_gwk_cipher *gwk_cipher;
	struct host_cmd_tlv_passphrase *passphrase;
	struct host_cmd_tlv_akmp *tlv_akmp;
	struct mwifiex_uap_bss_param *bss_cfg = cmd_buf;
	u16 cmd_size = *param_size;
	u8 *tlv = *tlv_buf;

	tlv_akmp = (struct host_cmd_tlv_akmp *)tlv;
	tlv_akmp->header.type = cpu_to_le16(TLV_TYPE_UAP_AKMP);
	tlv_akmp->header.len = cpu_to_le16(sizeof(struct host_cmd_tlv_akmp) -
					sizeof(struct mwifiex_ie_types_header));
	tlv_akmp->key_mgmt_operation = cpu_to_le16(bss_cfg->key_mgmt_operation);
	tlv_akmp->key_mgmt = cpu_to_le16(bss_cfg->key_mgmt);
	cmd_size += sizeof(struct host_cmd_tlv_akmp);
	tlv += sizeof(struct host_cmd_tlv_akmp);

	if (bss_cfg->wpa_cfg.pairwise_cipher_wpa & VALID_CIPHER_BITMAP) {
		pwk_cipher = (struct host_cmd_tlv_pwk_cipher *)tlv;
		pwk_cipher->header.type = cpu_to_le16(TLV_TYPE_PWK_CIPHER);
		pwk_cipher->header.len =
			cpu_to_le16(sizeof(struct host_cmd_tlv_pwk_cipher) -
				    sizeof(struct mwifiex_ie_types_header));
		pwk_cipher->proto = cpu_to_le16(PROTOCOL_WPA);
		pwk_cipher->cipher = bss_cfg->wpa_cfg.pairwise_cipher_wpa;
		cmd_size += sizeof(struct host_cmd_tlv_pwk_cipher);
		tlv += sizeof(struct host_cmd_tlv_pwk_cipher);
	}

	if (bss_cfg->wpa_cfg.pairwise_cipher_wpa2 & VALID_CIPHER_BITMAP) {
		pwk_cipher = (struct host_cmd_tlv_pwk_cipher *)tlv;
		pwk_cipher->header.type = cpu_to_le16(TLV_TYPE_PWK_CIPHER);
		pwk_cipher->header.len =
			cpu_to_le16(sizeof(struct host_cmd_tlv_pwk_cipher) -
				    sizeof(struct mwifiex_ie_types_header));
		pwk_cipher->proto = cpu_to_le16(PROTOCOL_WPA2);
		pwk_cipher->cipher = bss_cfg->wpa_cfg.pairwise_cipher_wpa2;
		cmd_size += sizeof(struct host_cmd_tlv_pwk_cipher);
		tlv += sizeof(struct host_cmd_tlv_pwk_cipher);
	}

	if (bss_cfg->wpa_cfg.group_cipher & VALID_CIPHER_BITMAP) {
		gwk_cipher = (struct host_cmd_tlv_gwk_cipher *)tlv;
		gwk_cipher->header.type = cpu_to_le16(TLV_TYPE_GWK_CIPHER);
		gwk_cipher->header.len =
			cpu_to_le16(sizeof(struct host_cmd_tlv_gwk_cipher) -
				    sizeof(struct mwifiex_ie_types_header));
		gwk_cipher->cipher = bss_cfg->wpa_cfg.group_cipher;
		cmd_size += sizeof(struct host_cmd_tlv_gwk_cipher);
		tlv += sizeof(struct host_cmd_tlv_gwk_cipher);
	}

	if (bss_cfg->wpa_cfg.length) {
		passphrase = (struct host_cmd_tlv_passphrase *)tlv;
		passphrase->header.type =
				cpu_to_le16(TLV_TYPE_UAP_WPA_PASSPHRASE);
		passphrase->header.len = cpu_to_le16(bss_cfg->wpa_cfg.length);
		memcpy(passphrase->passphrase, bss_cfg->wpa_cfg.passphrase,
		       bss_cfg->wpa_cfg.length);
		cmd_size += sizeof(struct mwifiex_ie_types_header) +
			    bss_cfg->wpa_cfg.length;
		tlv += sizeof(struct mwifiex_ie_types_header) +
				bss_cfg->wpa_cfg.length;
	}

	*param_size = cmd_size;
	*tlv_buf = tlv;

	return;
}