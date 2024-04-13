ipmi_get_channel_medium(struct ipmi_intf *intf, uint8_t channel)
{
	struct channel_info_t channel_info = {0};
	int ccode = 0;

	channel_info.channel = channel;
	ccode = _ipmi_get_channel_info(intf, &channel_info);
	if (ccode) {
		if (ccode != IPMI_CC_INV_DATA_FIELD_IN_REQ) {
			if (ccode > 0) {
				lprintf(LOG_ERR, "Get Channel Info command failed: %s",
				        val2str(ccode, completion_code_vals));
			}
			else {
				eval_ccode(ccode);
			}
		}
		return IPMI_CHANNEL_MEDIUM_RESERVED;
	}
	lprintf(LOG_DEBUG, "Channel type: %s",
			val2str(channel_info.medium, ipmi_channel_medium_vals));
	return channel_info.medium;
}