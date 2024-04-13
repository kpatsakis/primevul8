void mwifiex_set_vht_params(struct mwifiex_private *priv,
			    struct mwifiex_uap_bss_param *bss_cfg,
			    struct cfg80211_ap_settings *params)
{
	const u8 *vht_ie;

	vht_ie = cfg80211_find_ie(WLAN_EID_VHT_CAPABILITY, params->beacon.tail,
				  params->beacon.tail_len);
	if (vht_ie) {
		memcpy(&bss_cfg->vht_cap, vht_ie + 2,
		       sizeof(struct ieee80211_vht_cap));
		priv->ap_11ac_enabled = 1;
	} else {
		priv->ap_11ac_enabled = 0;
	}

	return;
}