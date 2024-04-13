void g_dhcpv6_client_create_iaid(GDHCPClient *dhcp_client, int index,
				unsigned char *iaid)
{
	uint8_t buf[6];

	__connman_inet_get_interface_mac_address(index, buf);

	memcpy(iaid, &buf[2], 4);
	dhcp_client->iaid = iaid[0] << 24 |
			iaid[1] << 16 | iaid[2] << 8 | iaid[3];
}