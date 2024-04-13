GDHCPClientError g_dhcp_client_set_id(GDHCPClient *dhcp_client)
{
	const unsigned maclen = 6;
	const unsigned idlen = maclen + 1;
	const uint8_t option_code = G_DHCP_CLIENT_ID;
	uint8_t idbuf[idlen];
	uint8_t *data_option;

	idbuf[0] = ARPHRD_ETHER;

	memcpy(&idbuf[1], dhcp_client->mac_address, maclen);

	data_option = alloc_dhcp_data_option(option_code, idbuf, idlen);
	if (!data_option)
		return G_DHCP_CLIENT_ERROR_NOMEM;

	g_hash_table_insert(dhcp_client->send_value_hash,
		GINT_TO_POINTER((int) option_code), data_option);

	return G_DHCP_CLIENT_ERROR_NONE;
}