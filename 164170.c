mwifiex_cmd_uap_sys_config(struct host_cmd_ds_command *cmd, u16 cmd_action,
			   u32 type, void *cmd_buf)
{
	u8 *tlv;
	u16 cmd_size, param_size, ie_size;
	struct host_cmd_ds_sys_config *sys_cfg;

	cmd->command = cpu_to_le16(HostCmd_CMD_UAP_SYS_CONFIG);
	cmd_size = (u16)(sizeof(struct host_cmd_ds_sys_config) + S_DS_GEN);
	sys_cfg = (struct host_cmd_ds_sys_config *)&cmd->params.uap_sys_config;
	sys_cfg->action = cpu_to_le16(cmd_action);
	tlv = sys_cfg->tlv;

	switch (type) {
	case UAP_BSS_PARAMS_I:
		param_size = cmd_size;
		if (mwifiex_uap_bss_param_prepare(tlv, cmd_buf, &param_size))
			return -1;
		cmd->size = cpu_to_le16(param_size);
		break;
	case UAP_CUSTOM_IE_I:
		ie_size = cmd_size;
		if (mwifiex_uap_custom_ie_prepare(tlv, cmd_buf, &ie_size))
			return -1;
		cmd->size = cpu_to_le16(ie_size);
		break;
	default:
		return -1;
	}

	return 0;
}