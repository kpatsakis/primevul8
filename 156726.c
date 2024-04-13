static void put_iaid(GDHCPClient *dhcp_client, int index, uint8_t *buf)
{
	uint32_t iaid;

	iaid = g_dhcpv6_client_get_iaid(dhcp_client);
	if (iaid == 0) {
		g_dhcpv6_client_create_iaid(dhcp_client, index, buf);
		return;
	}

	buf[0] = iaid >> 24;
	buf[1] = iaid >> 16;
	buf[2] = iaid >> 8;
	buf[3] = iaid;
}