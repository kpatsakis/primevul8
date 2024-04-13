mwifiex_uap_bss_wep(u8 **tlv_buf, void *cmd_buf, u16 *param_size)
{
	struct host_cmd_tlv_wep_key *wep_key;
	u16 cmd_size = *param_size;
	int i;
	u8 *tlv = *tlv_buf;
	struct mwifiex_uap_bss_param *bss_cfg = cmd_buf;

	for (i = 0; i < NUM_WEP_KEYS; i++) {
		if (bss_cfg->wep_cfg[i].length &&
		    (bss_cfg->wep_cfg[i].length == WLAN_KEY_LEN_WEP40 ||
		     bss_cfg->wep_cfg[i].length == WLAN_KEY_LEN_WEP104)) {
			wep_key = (struct host_cmd_tlv_wep_key *)tlv;
			wep_key->header.type =
				cpu_to_le16(TLV_TYPE_UAP_WEP_KEY);
			wep_key->header.len =
				cpu_to_le16(bss_cfg->wep_cfg[i].length + 2);
			wep_key->key_index = bss_cfg->wep_cfg[i].key_index;
			wep_key->is_default = bss_cfg->wep_cfg[i].is_default;
			memcpy(wep_key->key, bss_cfg->wep_cfg[i].key,
			       bss_cfg->wep_cfg[i].length);
			cmd_size += sizeof(struct mwifiex_ie_types_header) + 2 +
				    bss_cfg->wep_cfg[i].length;
			tlv += sizeof(struct mwifiex_ie_types_header) + 2 +
				    bss_cfg->wep_cfg[i].length;
		}
	}

	*param_size = cmd_size;
	*tlv_buf = tlv;

	return;
}