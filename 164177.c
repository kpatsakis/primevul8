int mwifiex_config_start_uap(struct mwifiex_private *priv,
			     struct mwifiex_uap_bss_param *bss_cfg)
{
	if (mwifiex_send_cmd(priv, HostCmd_CMD_UAP_SYS_CONFIG,
			     HostCmd_ACT_GEN_SET,
			     UAP_BSS_PARAMS_I, bss_cfg, true)) {
		mwifiex_dbg(priv->adapter, ERROR,
			    "Failed to set AP configuration\n");
		return -1;
	}

	if (mwifiex_send_cmd(priv, HostCmd_CMD_UAP_BSS_START,
			     HostCmd_ACT_GEN_SET, 0, NULL, true)) {
		mwifiex_dbg(priv->adapter, ERROR,
			    "Failed to start the BSS\n");
		return -1;
	}

	if (priv->sec_info.wep_enabled)
		priv->curr_pkt_filter |= HostCmd_ACT_MAC_WEP_ENABLE;
	else
		priv->curr_pkt_filter &= ~HostCmd_ACT_MAC_WEP_ENABLE;

	if (mwifiex_send_cmd(priv, HostCmd_CMD_MAC_CONTROL,
			     HostCmd_ACT_GEN_SET, 0,
			     &priv->curr_pkt_filter, true))
		return -1;

	return 0;
}