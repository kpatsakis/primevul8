int g_dhcpv6_client_set_ias(GDHCPClient *dhcp_client, int index,
			int code, uint32_t *T1, uint32_t *T2,
			GSList *addresses)
{
	GSList *list;
	uint8_t *ia_options, *pos;
	int len, count, total_len;

	count = g_slist_length(addresses);
	if (count == 0)
		return -EINVAL;

	g_dhcp_client_set_request(dhcp_client, code);

	if (code == G_DHCPV6_IA_TA)
		len = 4;         /* IAID */
	else if (code == G_DHCPV6_IA_NA)
		len = 4 + 4 + 4; /* IAID + T1 + T2 */
	else
		return -EINVAL;

	total_len = len + count * (2 + 2 + 16 + 4 + 4);
	ia_options = g_try_malloc0(total_len);
	if (!ia_options)
		return -ENOMEM;

	put_iaid(dhcp_client, index, ia_options);

	pos = &ia_options[len]; /* skip the IA_NA or IA_TA */

	for (list = addresses; list; list = list->next) {
		pos = append_iaaddr(dhcp_client, pos, list->data);
		if (!pos)
			break;
	}

	if (code == G_DHCPV6_IA_NA) {
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
	}

	g_dhcpv6_client_set_send(dhcp_client, code, ia_options, total_len);

	g_free(ia_options);

	return 0;
}