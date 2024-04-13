GDHCPClientError g_dhcp_client_set_send(GDHCPClient *dhcp_client,
		unsigned char option_code, const char *option_value)
{
	uint8_t *binary_option;

	if ((option_code == G_DHCP_HOST_NAME ||
		option_code == G_DHCP_VENDOR_CLASS_ID) && option_value) {
		binary_option = alloc_dhcp_string_option(option_code,
							option_value);
		if (!binary_option)
			return G_DHCP_CLIENT_ERROR_NOMEM;

		g_hash_table_insert(dhcp_client->send_value_hash,
			GINT_TO_POINTER((int) option_code), binary_option);
	}

	return G_DHCP_CLIENT_ERROR_NONE;
}