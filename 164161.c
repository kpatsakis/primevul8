mwifiex_set_ht_params(struct mwifiex_private *priv,
		      struct mwifiex_uap_bss_param *bss_cfg,
		      struct cfg80211_ap_settings *params)
{
	const u8 *ht_ie;

	if (!ISSUPP_11NENABLED(priv->adapter->fw_cap_info))
		return;

	ht_ie = cfg80211_find_ie(WLAN_EID_HT_CAPABILITY, params->beacon.tail,
				 params->beacon.tail_len);
	if (ht_ie) {
		memcpy(&bss_cfg->ht_cap, ht_ie + 2,
		       sizeof(struct ieee80211_ht_cap));
		priv->ap_11n_enabled = 1;
	} else {
		memset(&bss_cfg->ht_cap, 0, sizeof(struct ieee80211_ht_cap));
		bss_cfg->ht_cap.cap_info = cpu_to_le16(MWIFIEX_DEF_HT_CAP);
		bss_cfg->ht_cap.ampdu_params_info = MWIFIEX_DEF_AMPDU;
	}

	return;
}