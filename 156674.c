static void get_dhcpv6_request(GDHCPClient *dhcp_client,
				struct dhcpv6_packet *packet,
				uint16_t pkt_len, uint16_t *status)
{
	GList *list, *value_list;
	uint8_t *option;
	uint16_t code;
	uint16_t option_len;

	for (list = dhcp_client->request_list; list; list = list->next) {
		code = (uint16_t) GPOINTER_TO_INT(list->data);

		option = dhcpv6_get_option(packet, pkt_len, code, &option_len,
						NULL);
		if (!option) {
			g_hash_table_remove(dhcp_client->code_value_hash,
						GINT_TO_POINTER((int) code));
			continue;
		}

		value_list = get_dhcpv6_option_value_list(dhcp_client, code,
						option_len, option, status);

		debug(dhcp_client, "code %d %p len %d list %p", code, option,
			option_len, value_list);

		if (!value_list)
			g_hash_table_remove(dhcp_client->code_value_hash,
						GINT_TO_POINTER((int) code));
		else
			g_hash_table_insert(dhcp_client->code_value_hash,
				GINT_TO_POINTER((int) code), value_list);
	}
}