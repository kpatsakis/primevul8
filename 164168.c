void mwifiex_set_vht_width(struct mwifiex_private *priv,
			   enum nl80211_chan_width width,
			   bool ap_11ac_enable)
{
	struct mwifiex_adapter *adapter = priv->adapter;
	struct mwifiex_11ac_vht_cfg vht_cfg;

	vht_cfg.band_config = VHT_CFG_5GHZ;
	vht_cfg.cap_info = adapter->hw_dot_11ac_dev_cap;

	if (!ap_11ac_enable) {
		vht_cfg.mcs_tx_set = DISABLE_VHT_MCS_SET;
		vht_cfg.mcs_rx_set = DISABLE_VHT_MCS_SET;
	} else {
		vht_cfg.mcs_tx_set = DEFAULT_VHT_MCS_SET;
		vht_cfg.mcs_rx_set = DEFAULT_VHT_MCS_SET;
	}

	vht_cfg.misc_config  = VHT_CAP_UAP_ONLY;

	if (ap_11ac_enable && width >= NL80211_CHAN_WIDTH_80)
		vht_cfg.misc_config |= VHT_BW_80_160_80P80;

	mwifiex_send_cmd(priv, HostCmd_CMD_11AC_CFG,
			 HostCmd_ACT_GEN_SET, 0, &vht_cfg, true);

	return;
}