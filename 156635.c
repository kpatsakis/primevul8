static int send_dhcpv6_request(GDHCPClient *dhcp_client)
{
	return send_dhcpv6_msg(dhcp_client, DHCPV6_REQUEST, "request");
}