static int mwifiex_cmd_uap_sta_deauth(struct mwifiex_private *priv,
				      struct host_cmd_ds_command *cmd, u8 *mac)
{
	struct host_cmd_ds_sta_deauth *sta_deauth = &cmd->params.sta_deauth;

	cmd->command = cpu_to_le16(HostCmd_CMD_UAP_STA_DEAUTH);
	memcpy(sta_deauth->mac, mac, ETH_ALEN);
	sta_deauth->reason = cpu_to_le16(WLAN_REASON_DEAUTH_LEAVING);

	cmd->size = cpu_to_le16(sizeof(struct host_cmd_ds_sta_deauth) +
				S_DS_GEN);
	return 0;
}