static int send_solicitation(GDHCPClient *dhcp_client)
{
	return send_dhcpv6_msg(dhcp_client, DHCPV6_SOLICIT, "solicit");
}