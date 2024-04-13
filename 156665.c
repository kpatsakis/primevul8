int g_dhcpv6_client_set_ia(GDHCPClient *dhcp_client, int index,
			int code, uint32_t *T1, uint32_t *T2,
			bool add_iaaddr, const char *ia_na)
{
	if (code == G_DHCPV6_IA_TA) {
		uint8_t ia_options[4];

		put_iaid(dhcp_client, index, ia_options);

		g_dhcp_client_set_request(dhcp_client, G_DHCPV6_IA_TA);
		g_dhcpv6_client_set_send(dhcp_client, G_DHCPV6_IA_TA,
					ia_options, sizeof(ia_options));

	} else if (code == G_DHCPV6_IA_NA) {
		struct in6_addr addr;

		g_dhcp_client_set_request(dhcp_client, G_DHCPV6_IA_NA);

		/*
		 * If caller has specified the IPv6 address it wishes to
		 * to use (ia_na != NULL and address is valid), then send
		 * the address to server.
		 * If caller did not specify the address (ia_na == NULL) and
		 * if the current address is not set, then we should not send
		 * the address sub-option.
		 */
		if (add_iaaddr && ((!ia_na &&
			!IN6_IS_ADDR_UNSPECIFIED(&dhcp_client->ia_na))
			|| (ia_na &&
				inet_pton(AF_INET6, ia_na, &addr) == 1))) {
#define IAADDR_LEN (16+4+4)
			uint8_t ia_options[4+4+4+2+2+IAADDR_LEN];

			if (ia_na)
				memcpy(&dhcp_client->ia_na, &addr,
						sizeof(struct in6_addr));

			put_iaid(dhcp_client, index, ia_options);

			if (T1) {
				ia_options[4] = *T1 >> 24;
				ia_options[5] = *T1 >> 16;
				ia_options[6] = *T1 >> 8;
				ia_options[7] = *T1;
			} else
				memset(&ia_options[4], 0x00, 4);

			if (T2) {
				ia_options[8] = *T2 >> 24;
				ia_options[9] = *T2 >> 16;
				ia_options[10] = *T2 >> 8;
				ia_options[11] = *T2;
			} else
				memset(&ia_options[8], 0x00, 4);

			create_iaaddr(dhcp_client, &ia_options[12],
					IAADDR_LEN);

			g_dhcpv6_client_set_send(dhcp_client, G_DHCPV6_IA_NA,
					ia_options, sizeof(ia_options));
		} else {
			uint8_t ia_options[4+4+4];

			put_iaid(dhcp_client, index, ia_options);

			memset(&ia_options[4], 0x00, 4); /* T1 (4 bytes) */
			memset(&ia_options[8], 0x00, 4); /* T2 (4 bytes) */

			g_dhcpv6_client_set_send(dhcp_client, G_DHCPV6_IA_NA,
					ia_options, sizeof(ia_options));
		}

	} else
		return -EINVAL;

	return 0;
}