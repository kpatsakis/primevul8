static int mwifiex_uap_parse_tail_ies(struct mwifiex_private *priv,
				      struct cfg80211_beacon_data *info)
{
	struct mwifiex_ie *gen_ie;
	struct ieee_types_header *hdr;
	struct ieee80211_vendor_ie *vendorhdr;
	u16 gen_idx = MWIFIEX_AUTO_IDX_MASK, ie_len = 0;
	int left_len, parsed_len = 0;
	unsigned int token_len;
	int err = 0;

	if (!info->tail || !info->tail_len)
		return 0;

	gen_ie = kzalloc(sizeof(*gen_ie), GFP_KERNEL);
	if (!gen_ie)
		return -ENOMEM;

	left_len = info->tail_len;

	/* Many IEs are generated in FW by parsing bss configuration.
	 * Let's not add them here; else we may end up duplicating these IEs
	 */
	while (left_len > sizeof(struct ieee_types_header)) {
		hdr = (void *)(info->tail + parsed_len);
		token_len = hdr->len + sizeof(struct ieee_types_header);
		if (token_len > left_len) {
			err = -EINVAL;
			goto out;
		}

		switch (hdr->element_id) {
		case WLAN_EID_SSID:
		case WLAN_EID_SUPP_RATES:
		case WLAN_EID_COUNTRY:
		case WLAN_EID_PWR_CONSTRAINT:
		case WLAN_EID_ERP_INFO:
		case WLAN_EID_EXT_SUPP_RATES:
		case WLAN_EID_HT_CAPABILITY:
		case WLAN_EID_HT_OPERATION:
		case WLAN_EID_VHT_CAPABILITY:
		case WLAN_EID_VHT_OPERATION:
			break;
		case WLAN_EID_VENDOR_SPECIFIC:
			/* Skip only Microsoft WMM IE */
			if (cfg80211_find_vendor_ie(WLAN_OUI_MICROSOFT,
						    WLAN_OUI_TYPE_MICROSOFT_WMM,
						    (const u8 *)hdr,
						    token_len))
				break;
			/* fall through */
		default:
			if (ie_len + token_len > IEEE_MAX_IE_SIZE) {
				err = -EINVAL;
				goto out;
			}
			memcpy(gen_ie->ie_buffer + ie_len, hdr, token_len);
			ie_len += token_len;
			break;
		}
		left_len -= token_len;
		parsed_len += token_len;
	}

	/* parse only WPA vendor IE from tail, WMM IE is configured by
	 * bss_config command
	 */
	vendorhdr = (void *)cfg80211_find_vendor_ie(WLAN_OUI_MICROSOFT,
						    WLAN_OUI_TYPE_MICROSOFT_WPA,
						    info->tail, info->tail_len);
	if (vendorhdr) {
		token_len = vendorhdr->len + sizeof(struct ieee_types_header);
		if (ie_len + token_len > IEEE_MAX_IE_SIZE) {
			err = -EINVAL;
			goto out;
		}
		memcpy(gen_ie->ie_buffer + ie_len, vendorhdr, token_len);
		ie_len += token_len;
	}

	if (!ie_len)
		goto out;

	gen_ie->ie_index = cpu_to_le16(gen_idx);
	gen_ie->mgmt_subtype_mask = cpu_to_le16(MGMT_MASK_BEACON |
						MGMT_MASK_PROBE_RESP |
						MGMT_MASK_ASSOC_RESP);
	gen_ie->ie_length = cpu_to_le16(ie_len);

	if (mwifiex_update_uap_custom_ie(priv, gen_ie, &gen_idx, NULL, NULL,
					 NULL, NULL)) {
		err = -EINVAL;
		goto out;
	}

	priv->gen_idx = gen_idx;

 out:
	kfree(gen_ie);
	return err;
}