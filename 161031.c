ipmi_get_channel_info(struct ipmi_intf *intf, uint8_t channel)
{
	struct channel_info_t channel_info = {0};
	struct channel_access_t channel_access = {0};
	int ccode = 0;

	channel_info.channel = channel;
	ccode = _ipmi_get_channel_info(intf, &channel_info);
	if (eval_ccode(ccode) != 0) {
		lprintf(LOG_ERR, "Unable to Get Channel Info");
		return (-1);
	}

	printf("Channel 0x%x info:\n", channel_info.channel);
	printf("  Channel Medium Type   : %s\n",
		   val2str(channel_info.medium,
			   ipmi_channel_medium_vals));
	printf("  Channel Protocol Type : %s\n",
		   val2str(channel_info.protocol,
			   ipmi_channel_protocol_vals));
	printf("  Session Support       : ");
	switch (channel_info.session_support) {
		case IPMI_CHANNEL_SESSION_LESS:
			printf("session-less\n");
			break;
		case IPMI_CHANNEL_SESSION_SINGLE:
			printf("single-session\n");
			break;
		case IPMI_CHANNEL_SESSION_MULTI:
			printf("multi-session\n");
			break;
		case IPMI_CHANNEL_SESSION_BASED:
			printf("session-based\n");
			break;
		default:
			printf("unknown\n");
			break;
	}
	printf("  Active Session Count  : %d\n",
		   channel_info.active_sessions);
	printf("  Protocol Vendor ID    : %d\n",
		   channel_info.vendor_id[0]      |
		   channel_info.vendor_id[1] << 8 |
		   channel_info.vendor_id[2] << 16);

	/* only proceed if this is LAN channel */
	if (channel_info.medium != IPMI_CHANNEL_MEDIUM_LAN
		&& channel_info.medium != IPMI_CHANNEL_MEDIUM_LAN_OTHER) {
		return 0;
	}

	channel_access.channel = channel_info.channel;
	ccode = _ipmi_get_channel_access(intf, &channel_access, 1);
	if (eval_ccode(ccode) != 0) {
		lprintf(LOG_ERR, "Unable to Get Channel Access (volatile)");
		return (-1);
	}

	printf("  Volatile(active) Settings\n");
	printf("    Alerting            : %sabled\n",
			(channel_access.alerting) ? "dis" : "en");
	printf("    Per-message Auth    : %sabled\n",
			(channel_access.per_message_auth) ? "dis" : "en");
	printf("    User Level Auth     : %sabled\n",
			(channel_access.user_level_auth) ? "dis" : "en");
	printf("    Access Mode         : ");
	switch (channel_access.access_mode) {
		case 0:
			printf("disabled\n");
			break;
		case 1:
			printf("pre-boot only\n");
			break;
		case 2:
			printf("always available\n");
			break;
		case 3:
			printf("shared\n");
			break;
		default:
			printf("unknown\n");
			break;
	}

	memset(&channel_access, 0, sizeof(channel_access));
	channel_access.channel = channel_info.channel;
	/* get non-volatile settings */
	ccode = _ipmi_get_channel_access(intf, &channel_access, 0);
	if (eval_ccode(ccode) != 0) {
		lprintf(LOG_ERR, "Unable to Get Channel Access (non-volatile)");
		return (-1);
	}

	printf("  Non-Volatile Settings\n");
	printf("    Alerting            : %sabled\n",
			(channel_access.alerting) ? "dis" : "en");
	printf("    Per-message Auth    : %sabled\n",
			(channel_access.per_message_auth) ? "dis" : "en");
	printf("    User Level Auth     : %sabled\n",
			(channel_access.user_level_auth) ? "dis" : "en");
	printf("    Access Mode         : ");
	switch (channel_access.access_mode) {
		case 0:
			printf("disabled\n");
			break;
		case 1:
			printf("pre-boot only\n");
			break;
		case 2:
			printf("always available\n");
			break;
		case 3:
			printf("shared\n");
			break;
		default:
			printf("unknown\n");
			break;
	}
	return 0;
}