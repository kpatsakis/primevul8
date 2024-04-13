static int send_information_req(GDHCPClient *dhcp_client)
{
	return send_dhcpv6_msg(dhcp_client, DHCPV6_INFORMATION_REQ,
				"information-req");
}