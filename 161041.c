_ipmi_get_channel_access(struct ipmi_intf *intf,
		struct channel_access_t *channel_access,
		uint8_t get_volatile_settings)
{
	struct ipmi_rs *rsp;
	struct ipmi_rq req = {0};
	uint8_t data[2];

	if (!channel_access) {
		return (-3);
	}
	data[0] = channel_access->channel & 0x0F;
	/* volatile - 0x80; non-volatile - 0x40 */
	data[1] = get_volatile_settings ? 0x80 : 0x40;
	req.msg.netfn = IPMI_NETFN_APP;
	req.msg.cmd = IPMI_GET_CHANNEL_ACCESS;
	req.msg.data = data;
	req.msg.data_len = 2;

	rsp = intf->sendrecv(intf, &req);
	if (!rsp) {
		return (-1);
	} else if (rsp->ccode) {
		return rsp->ccode;
	} else if (rsp->data_len != 2) {
		return (-2);
	}
	channel_access->alerting = rsp->data[0] & 0x20;
	channel_access->per_message_auth = rsp->data[0] & 0x10;
	channel_access->user_level_auth = rsp->data[0] & 0x08;
	channel_access->access_mode = rsp->data[0] & 0x07;
	channel_access->privilege_limit = rsp->data[1] & 0x0F;
	return 0;
}