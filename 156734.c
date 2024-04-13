char *g_dhcp_client_get_address(GDHCPClient *dhcp_client)
{
	return g_strdup(dhcp_client->assigned_ip);
}