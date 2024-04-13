static void add_dhcpv6_request_options(GDHCPClient *dhcp_client,
				struct dhcpv6_packet *packet,
				unsigned char *buf, int max_buf,
				unsigned char **ptr_buf)
{
	GList *list;
	uint16_t code, value;
	bool added;
	int32_t diff;
	int len;

	if (dhcp_client->type != G_DHCP_IPV6)
		return;

	for (list = dhcp_client->request_list; list; list = list->next) {
		code = (uint16_t) GPOINTER_TO_INT(list->data);
		added = false;

		switch (code) {
		case G_DHCPV6_CLIENTID:
			if (!dhcp_client->duid)
				return;

			len = 2 + 2 + dhcp_client->duid_len;
			if ((*ptr_buf + len) > (buf + max_buf)) {
				debug(dhcp_client, "Too long dhcpv6 message "
					"when writing client id option");
				return;
			}

			copy_option(*ptr_buf, G_DHCPV6_CLIENTID,
				dhcp_client->duid_len, dhcp_client->duid);
			(*ptr_buf) += len;
			added = true;
			break;

		case G_DHCPV6_SERVERID:
			if (!dhcp_client->server_duid)
				break;

			len = 2 + 2 + dhcp_client->server_duid_len;
			if ((*ptr_buf + len) > (buf + max_buf)) {
				debug(dhcp_client, "Too long dhcpv6 message "
					"when writing server id option");
				return;
			}

			copy_option(*ptr_buf, G_DHCPV6_SERVERID,
				dhcp_client->server_duid_len,
				dhcp_client->server_duid);
			(*ptr_buf) += len;
			added = true;
			break;

		case G_DHCPV6_RAPID_COMMIT:
			len = 2 + 2;
			if ((*ptr_buf + len) > (buf + max_buf)) {
				debug(dhcp_client, "Too long dhcpv6 message "
					"when writing rapid commit option");
				return;
			}

			copy_option(*ptr_buf, G_DHCPV6_RAPID_COMMIT, 0, 0);
			(*ptr_buf) += len;
			added = true;
			break;

		case G_DHCPV6_ORO:
			break;

		case G_DHCPV6_ELAPSED_TIME:
			if (!dhcp_client->retransmit) {
				/*
				 * Initial message, elapsed time is 0.
				 */
				diff = 0;
			} else {
				diff = get_time_diff(&dhcp_client->start_time);
				if (diff < 0 || diff > 0xffff)
					diff = 0xffff;
			}

			len = 2 + 2 + 2;
			if ((*ptr_buf + len) > (buf + max_buf)) {
				debug(dhcp_client, "Too long dhcpv6 message "
					"when writing elapsed time option");
				return;
			}

			value = htons((uint16_t)diff);
			copy_option(*ptr_buf, G_DHCPV6_ELAPSED_TIME,
				2, (uint8_t *)&value);
			(*ptr_buf) += len;
			added = true;
			break;

		case G_DHCPV6_DNS_SERVERS:
			break;

		case G_DHCPV6_DOMAIN_LIST:
			break;

		case G_DHCPV6_SNTP_SERVERS:
			break;

		default:
			break;
		}

		if (added)
			debug(dhcp_client, "option %d len %d added", code, len);
	}
}