void g_dhcpv6_client_set_iaid(GDHCPClient *dhcp_client, uint32_t iaid)
{
	if (!dhcp_client || dhcp_client->type != G_DHCP_IPV6)
		return;

	dhcp_client->iaid = iaid;
}