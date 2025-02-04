mwifiex_set_uap_rates(struct mwifiex_uap_bss_param *bss_cfg,
		      struct cfg80211_ap_settings *params)
{
	struct ieee_types_header *rate_ie;
	int var_offset = offsetof(struct ieee80211_mgmt, u.beacon.variable);
	const u8 *var_pos = params->beacon.head + var_offset;
	int len = params->beacon.head_len - var_offset;
	u8 rate_len = 0;

	rate_ie = (void *)cfg80211_find_ie(WLAN_EID_SUPP_RATES, var_pos, len);
	if (rate_ie) {
		if (rate_ie->len > MWIFIEX_SUPPORTED_RATES)
			return;
		memcpy(bss_cfg->rates, rate_ie + 1, rate_ie->len);
		rate_len = rate_ie->len;
	}

	rate_ie = (void *)cfg80211_find_ie(WLAN_EID_EXT_SUPP_RATES,
					   params->beacon.tail,
					   params->beacon.tail_len);
	if (rate_ie) {
		if (rate_ie->len > MWIFIEX_SUPPORTED_RATES - rate_len)
			return;
		memcpy(bss_cfg->rates + rate_len, rate_ie + 1, rate_ie->len);
	}

	return;
}