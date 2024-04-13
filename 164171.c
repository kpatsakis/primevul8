void mwifiex_set_tpc_params(struct mwifiex_private *priv,
			    struct mwifiex_uap_bss_param *bss_cfg,
			    struct cfg80211_ap_settings *params)
{
	const u8 *tpc_ie;

	tpc_ie = cfg80211_find_ie(WLAN_EID_TPC_REQUEST, params->beacon.tail,
				  params->beacon.tail_len);
	if (tpc_ie)
		bss_cfg->power_constraint = *(tpc_ie + 2);
	else
		bss_cfg->power_constraint = 0;
}