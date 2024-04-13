ipmi_get_channel_auth_cap(struct ipmi_intf *intf, uint8_t channel, uint8_t priv)
{
	struct ipmi_rs *rsp;
	struct ipmi_rq req;
	struct get_channel_auth_cap_rsp auth_cap;
	uint8_t msg_data[2];

	/* Ask for IPMI v2 data as well */
	msg_data[0] = channel | 0x80;
	msg_data[1] = priv;

	memset(&req, 0, sizeof(req));
	req.msg.netfn = IPMI_NETFN_APP;
	req.msg.cmd = IPMI_GET_CHANNEL_AUTH_CAP;
	req.msg.data = msg_data;
	req.msg.data_len = 2;

	rsp = intf->sendrecv(intf, &req);

	if (!rsp || rsp->ccode) {
		/*
		 * It's very possible that this failed because we asked for IPMI v2 data
		 * Ask again, without requesting IPMI v2 data
		 */
		msg_data[0] &= 0x7F;
		
		rsp = intf->sendrecv(intf, &req);
		if (!rsp) {
			lprintf(LOG_ERR, "Unable to Get Channel Authentication Capabilities");
			return (-1);
		}
		if (rsp->ccode) {
			lprintf(LOG_ERR, "Get Channel Authentication Capabilities failed: %s",
				val2str(rsp->ccode, completion_code_vals));
			return (-1);
		}
	}

	memcpy(&auth_cap, rsp->data, sizeof(struct get_channel_auth_cap_rsp));

	printf("Channel number             : %d\n",
		   auth_cap.channel_number);
	printf("IPMI v1.5  auth types      : %s\n",
		   ipmi_1_5_authtypes(auth_cap.enabled_auth_types));

	if (auth_cap.v20_data_available) {
		printf("KG status                  : %s\n",
			   (auth_cap.kg_status) ? "non-zero" : "default (all zeroes)");
	}

	printf("Per message authentication : %sabled\n",
		   (auth_cap.per_message_auth) ? "dis" : "en");
	printf("User level authentication  : %sabled\n",
		   (auth_cap.user_level_auth) ? "dis" : "en");

	printf("Non-null user names exist  : %s\n",
		   (auth_cap.non_null_usernames) ? "yes" : "no");
	printf("Null user names exist      : %s\n",
		   (auth_cap.null_usernames) ? "yes" : "no");
	printf("Anonymous login enabled    : %s\n",
		   (auth_cap.anon_login_enabled) ? "yes" : "no");

	if (auth_cap.v20_data_available) {
		printf("Channel supports IPMI v1.5 : %s\n",
			   (auth_cap.ipmiv15_support) ? "yes" : "no");
		printf("Channel supports IPMI v2.0 : %s\n",
			   (auth_cap.ipmiv20_support) ? "yes" : "no");
	}

	/*
	 * If there is support for an OEM authentication type, there is some
	 * information.
	 */
	if (auth_cap.enabled_auth_types & IPMI_1_5_AUTH_TYPE_BIT_OEM) {
		printf("IANA Number for OEM        : %d\n",
			   auth_cap.oem_id[0]      | 
			   auth_cap.oem_id[1] << 8 | 
			   auth_cap.oem_id[2] << 16);
		printf("OEM Auxiliary Data         : 0x%x\n",
			   auth_cap.oem_aux_data);
	}

	return 0;
}