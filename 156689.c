int g_dhcpv6_client_set_pd(GDHCPClient *dhcp_client, uint32_t *T1,
			uint32_t *T2, GSList *prefixes)
{
	uint8_t options[1452];
	unsigned int max_buf = sizeof(options);
	int len, count = g_slist_length(prefixes);

	if (!dhcp_client || dhcp_client->type != G_DHCP_IPV6)
		return -EINVAL;

	g_dhcp_client_set_request(dhcp_client, G_DHCPV6_IA_PD);

	memset(options, 0, sizeof(options));

	options[0] = dhcp_client->iaid >> 24;
	options[1] = dhcp_client->iaid >> 16;
	options[2] = dhcp_client->iaid >> 8;
	options[3] = dhcp_client->iaid;

	if (T1) {
		uint32_t t = htonl(*T1);
		memcpy(&options[4], &t, 4);
	}

	if (T2) {
		uint32_t t = htonl(*T2);
		memcpy(&options[8], &t, 4);
	}

	len = 12;

	if (count > 0) {
		GSList *list;

		for (list = prefixes; list; list = list->next) {
			GDHCPIAPrefix *prefix = list->data;
			uint8_t sub_option[4+4+1+16];

			if ((len + 2 + 2 + sizeof(sub_option)) >= max_buf) {
				debug(dhcp_client,
					"Too long dhcpv6 message "
					"when writing IA prefix option");
				return -EINVAL;
			}

			memset(&sub_option, 0, sizeof(sub_option));

			/* preferred and validity time are left zero */

			sub_option[8] = prefix->prefixlen;
			memcpy(&sub_option[9], &prefix->prefix, 16);

			copy_option(&options[len], G_DHCPV6_IA_PREFIX,
				sizeof(sub_option), sub_option);
			len += 2 + 2 + sizeof(sub_option);
		}
	}

	g_dhcpv6_client_set_send(dhcp_client, G_DHCPV6_IA_PD,
				options, len);

	return 0;
}