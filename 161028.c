_ipmi_set_channel_access(struct ipmi_intf *intf,
		struct channel_access_t channel_access,
		uint8_t access_option,
		uint8_t privilege_option)
{
	struct ipmi_rs *rsp;
	struct ipmi_rq req;
	uint8_t data[3];
	/* Only values from <0..2> are accepted as valid. */
	if (access_option > 2 || privilege_option > 2) {
		return (-3);
	}

	memset(&data, 0, sizeof(data));
	data[0] = channel_access.channel & 0x0F;
	data[1] = (access_option << 6);
	if (channel_access.alerting) {
		data[1] |= 0x20;
	}
	if (channel_access.per_message_auth) {
		data[1] |= 0x10;
	}
	if (channel_access.user_level_auth) {
		data[1] |= 0x08;
	}
	data[1] |= (channel_access.access_mode & 0x07);
	data[2] = (privilege_option << 6);
	data[2] |= (channel_access.privilege_limit & 0x0F);

	memset(&req, 0, sizeof(req));
	req.msg.netfn = IPMI_NETFN_APP;
	req.msg.cmd = IPMI_SET_CHANNEL_ACCESS;
	req.msg.data = data;
	req.msg.data_len = 3;
	
	rsp = intf->sendrecv(intf, &req);
	if (!rsp) {
		return (-1);
	}
	return rsp->ccode;
}