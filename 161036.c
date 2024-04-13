ipmi_current_channel_info(struct ipmi_intf *intf,
                          struct channel_info_t *chinfo)
{
	int ccode = 0;

	chinfo->channel = CH_CURRENT;
	ccode = _ipmi_get_channel_info(intf, chinfo);
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
		chinfo->channel = CH_UNKNOWN;
		chinfo->medium = IPMI_CHANNEL_MEDIUM_RESERVED;
	}
	return;
}