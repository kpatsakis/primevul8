_ipmi_get_channel_info(struct ipmi_intf *intf,
		struct channel_info_t *channel_info)
{
	struct ipmi_rs *rsp;
	struct ipmi_rq req = {0};
	uint8_t data[1];

	if (!channel_info) {
		return (-3);
	}
	data[0] = channel_info->channel & 0x0F;
	req.msg.netfn = IPMI_NETFN_APP;
	req.msg.cmd = IPMI_GET_CHANNEL_INFO;
	req.msg.data = data;
	req.msg.data_len = 1;

	rsp = intf->sendrecv(intf, &req);
	if (!rsp) {
		return (-1);
	} else if (rsp->ccode) {
		return rsp->ccode;
	} else if (rsp->data_len != 9) {
		return (-2);
	}
	channel_info->channel = rsp->data[0] & 0x0F;
	channel_info->medium = rsp->data[1] & 0x7F;
	channel_info->protocol = rsp->data[2] & 0x1F;
	channel_info->session_support = rsp->data[3] & 0xC0;
	channel_info->active_sessions = rsp->data[3] & 0x3F;
	memcpy(channel_info->vendor_id, &rsp->data[4],
			sizeof(channel_info->vendor_id));
	memcpy(channel_info->aux_info, &rsp->data[7],
			sizeof(channel_info->aux_info));
	return 0;
}