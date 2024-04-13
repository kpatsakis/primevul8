void g_dhcpv6_client_set_retransmit(GDHCPClient *dhcp_client)
{
	if (!dhcp_client)
		return;

	dhcp_client->retransmit = true;
}