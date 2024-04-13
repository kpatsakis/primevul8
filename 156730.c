uint32_t g_dhcpv6_client_get_iaid(GDHCPClient *dhcp_client)
{
	if (!dhcp_client || dhcp_client->type != G_DHCP_IPV6)
		return 0;

	return dhcp_client->iaid;
}