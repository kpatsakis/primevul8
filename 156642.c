GDHCPClientError g_dhcp_client_set_request(GDHCPClient *dhcp_client,
						unsigned int option_code)
{
	if (!g_list_find(dhcp_client->request_list,
				GINT_TO_POINTER((int)option_code)))
		dhcp_client->request_list = g_list_prepend(
					dhcp_client->request_list,
					(GINT_TO_POINTER((int) option_code)));

	return G_DHCP_CLIENT_ERROR_NONE;
}