static int send_dhcpv6_release(GDHCPClient *dhcp_client)
{
	return send_dhcpv6_msg(dhcp_client, DHCPV6_RELEASE, "release");
}