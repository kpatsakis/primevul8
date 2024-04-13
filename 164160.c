static int mwifiex_set_mgmt_beacon_data_ies(struct mwifiex_private *priv,
					    struct cfg80211_beacon_data *data)
{
	struct mwifiex_ie *beacon_ie = NULL, *pr_ie = NULL, *ar_ie = NULL;
	u16 beacon_idx = MWIFIEX_AUTO_IDX_MASK, pr_idx = MWIFIEX_AUTO_IDX_MASK;
	u16 ar_idx = MWIFIEX_AUTO_IDX_MASK;
	int ret = 0;

	if (data->beacon_ies && data->beacon_ies_len) {
		mwifiex_update_vs_ie(data->beacon_ies, data->beacon_ies_len,
				     &beacon_ie, MGMT_MASK_BEACON,
				     WLAN_OUI_MICROSOFT,
				     WLAN_OUI_TYPE_MICROSOFT_WPS);
		mwifiex_update_vs_ie(data->beacon_ies, data->beacon_ies_len,
				     &beacon_ie, MGMT_MASK_BEACON,
				     WLAN_OUI_WFA, WLAN_OUI_TYPE_WFA_P2P);
	}

	if (data->proberesp_ies && data->proberesp_ies_len) {
		mwifiex_update_vs_ie(data->proberesp_ies,
				     data->proberesp_ies_len, &pr_ie,
				     MGMT_MASK_PROBE_RESP, WLAN_OUI_MICROSOFT,
				     WLAN_OUI_TYPE_MICROSOFT_WPS);
		mwifiex_update_vs_ie(data->proberesp_ies,
				     data->proberesp_ies_len, &pr_ie,
				     MGMT_MASK_PROBE_RESP,
				     WLAN_OUI_WFA, WLAN_OUI_TYPE_WFA_P2P);
	}

	if (data->assocresp_ies && data->assocresp_ies_len) {
		mwifiex_update_vs_ie(data->assocresp_ies,
				     data->assocresp_ies_len, &ar_ie,
				     MGMT_MASK_ASSOC_RESP |
				     MGMT_MASK_REASSOC_RESP,
				     WLAN_OUI_MICROSOFT,
				     WLAN_OUI_TYPE_MICROSOFT_WPS);
		mwifiex_update_vs_ie(data->assocresp_ies,
				     data->assocresp_ies_len, &ar_ie,
				     MGMT_MASK_ASSOC_RESP |
				     MGMT_MASK_REASSOC_RESP, WLAN_OUI_WFA,
				     WLAN_OUI_TYPE_WFA_P2P);
	}

	if (beacon_ie || pr_ie || ar_ie) {
		ret = mwifiex_update_uap_custom_ie(priv, beacon_ie,
						   &beacon_idx, pr_ie,
						   &pr_idx, ar_ie, &ar_idx);
		if (ret)
			goto done;
	}

	priv->beacon_idx = beacon_idx;
	priv->proberesp_idx = pr_idx;
	priv->assocresp_idx = ar_idx;

done:
	kfree(beacon_ie);
	kfree(pr_ie);
	kfree(ar_ie);

	return ret;
}