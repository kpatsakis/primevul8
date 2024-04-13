char *g_dhcp_client_get_server_address(GDHCPClient *dhcp_client)
{
	if (!dhcp_client)
		return NULL;

	return get_ip(dhcp_client->server_ip);
}