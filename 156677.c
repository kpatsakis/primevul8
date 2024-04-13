static GList *get_addresses(GDHCPClient *dhcp_client,
				int code, int len,
				unsigned char *value,
				uint16_t *status)
{
	GList *list = NULL;
	struct in6_addr addr;
	uint32_t iaid, T1 = 0, T2 = 0, preferred = 0, valid = 0;
	uint16_t option_len, option_code, st = 0, max_len;
	int addr_count = 0, prefix_count = 0, i, pos;
	unsigned char prefixlen;
	unsigned int shortest_valid = 0;
	uint8_t *option;
	char *str;

	if (!value || len < 4)
		return NULL;

	iaid = get_uint32(&value[0]);
	if (dhcp_client->iaid != iaid)
		return NULL;

	if (code == G_DHCPV6_IA_NA || code == G_DHCPV6_IA_PD) {
		T1 = get_uint32(&value[4]);
		T2 = get_uint32(&value[8]);

		if (T1 > T2)
			/* IA_NA: RFC 3315, 22.4 */
			/* IA_PD: RFC 3633, ch 9 */
			return NULL;

		pos = 12;
	} else
		pos = 4;

	if (len <= pos)
		return NULL;

	max_len = len - pos;

	debug(dhcp_client, "header %d sub-option max len %d", pos, max_len);

	/* We have more sub-options in this packet. */
	do {
		option = dhcpv6_get_sub_option(&value[pos], max_len,
					&option_code, &option_len);

		debug(dhcp_client, "pos %d option %p code %d len %d",
			pos, option, option_code, option_len);

		if (!option)
			break;

		if (pos >= len)
			break;

		switch (option_code) {
		case G_DHCPV6_IAADDR:
			i = 0;
			memcpy(&addr, &option[0], sizeof(addr));
			i += sizeof(addr);
			preferred = get_uint32(&option[i]);
			i += 4;
			valid = get_uint32(&option[i]);

			addr_count++;
			break;

		case G_DHCPV6_STATUS_CODE:
			st = get_uint16(&option[0]);
			debug(dhcp_client, "error code %d", st);
			if (option_len > 2) {
				str = g_strndup((gchar *)&option[2],
						option_len - 2);
				debug(dhcp_client, "error text: %s", str);
				g_free(str);
			}

			*status = st;
			break;

		case G_DHCPV6_IA_PREFIX:
			i = 0;
			preferred = get_uint32(&option[i]);
			i += 4;
			valid = get_uint32(&option[i]);
			i += 4;
			prefixlen = option[i];
			i += 1;
			memcpy(&addr, &option[i], sizeof(addr));
			i += sizeof(addr);
			if (preferred < valid) {
				/* RFC 3633, ch 10 */
				list = add_prefix(dhcp_client, list, &addr,
						prefixlen, preferred, valid);
				if (shortest_valid > valid)
					shortest_valid = valid;
				prefix_count++;
			}
			break;
		}

		pos += 2 + 2 + option_len;

	} while (pos < len);

	if (addr_count > 0 && st == 0) {
		/* We only support one address atm */
		char addr_str[INET6_ADDRSTRLEN + 1];

		if (preferred > valid)
			/* RFC 3315, 22.6 */
			return NULL;

		dhcp_client->T1 = T1;
		dhcp_client->T2 = T2;

		inet_ntop(AF_INET6, &addr, addr_str, INET6_ADDRSTRLEN);
		debug(dhcp_client, "address count %d addr %s T1 %u T2 %u",
			addr_count, addr_str, T1, T2);

		list = g_list_append(list, g_strdup(addr_str));

		if (code == G_DHCPV6_IA_NA)
			memcpy(&dhcp_client->ia_na, &addr,
						sizeof(struct in6_addr));
		else
			memcpy(&dhcp_client->ia_ta, &addr,
						sizeof(struct in6_addr));

		if (valid != dhcp_client->expire)
			dhcp_client->expire = valid;
	}

	if (prefix_count > 0 && list) {
		/*
		 * This means we have a list of prefixes to delegate.
		 */
		list = g_list_reverse(list);

		debug(dhcp_client, "prefix count %d T1 %u T2 %u",
			prefix_count, T1, T2);

		dhcp_client->T1 = T1;
		dhcp_client->T2 = T2;

		dhcp_client->expire = shortest_valid;
	}

	if (status && *status != 0)
		debug(dhcp_client, "status %d", *status);

	return list;
}