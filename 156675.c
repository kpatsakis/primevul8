static int send_dhcpv6_decline(GDHCPClient *dhcp_client)
{
	return send_dhcpv6_msg(dhcp_client, DHCPV6_DECLINE, "decline");
}