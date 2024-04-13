void g_dhcpv6_client_reset_request(GDHCPClient *dhcp_client)
{
	if (!dhcp_client || dhcp_client->type != G_DHCP_IPV6)
		return;

	dhcp_client->last_request = time(NULL);
}