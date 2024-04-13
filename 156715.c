void g_dhcpv6_client_set_send(GDHCPClient *dhcp_client,
					uint16_t option_code,
					uint8_t *option_value,
					uint16_t option_len)
{
	if (option_value) {
		uint8_t *binary_option;

		debug(dhcp_client, "setting option %d to %p len %d",
			option_code, option_value, option_len);

		binary_option = alloc_dhcpv6_option(option_code, option_value,
						option_len);
		if (binary_option)
			g_hash_table_insert(dhcp_client->send_value_hash,
					GINT_TO_POINTER((int) option_code),
					binary_option);
	}
}