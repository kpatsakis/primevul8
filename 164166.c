int mwifiex_uap_prepare_cmd(struct mwifiex_private *priv, u16 cmd_no,
			    u16 cmd_action, u32 type,
			    void *data_buf, void *cmd_buf)
{
	struct host_cmd_ds_command *cmd = cmd_buf;

	switch (cmd_no) {
	case HostCmd_CMD_UAP_SYS_CONFIG:
		if (mwifiex_cmd_uap_sys_config(cmd, cmd_action, type, data_buf))
			return -1;
		break;
	case HostCmd_CMD_UAP_BSS_START:
	case HostCmd_CMD_UAP_BSS_STOP:
	case HOST_CMD_APCMD_SYS_RESET:
	case HOST_CMD_APCMD_STA_LIST:
		cmd->command = cpu_to_le16(cmd_no);
		cmd->size = cpu_to_le16(S_DS_GEN);
		break;
	case HostCmd_CMD_UAP_STA_DEAUTH:
		if (mwifiex_cmd_uap_sta_deauth(priv, cmd, data_buf))
			return -1;
		break;
	case HostCmd_CMD_CHAN_REPORT_REQUEST:
		if (mwifiex_cmd_issue_chan_report_request(priv, cmd_buf,
							  data_buf))
			return -1;
		break;
	default:
		mwifiex_dbg(priv->adapter, ERROR,
			    "PREP_CMD: unknown cmd %#x\n", cmd_no);
		return -1;
	}

	return 0;
}