static int send_dhcpv6_confirm(GDHCPClient *dhcp_client)
{
	return send_dhcpv6_msg(dhcp_client, DHCPV6_CONFIRM, "confirm");
}