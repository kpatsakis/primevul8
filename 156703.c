static uint8_t *create_iaaddr(GDHCPClient *dhcp_client, uint8_t *buf,
				uint16_t len)
{
	buf[0] = 0;
	buf[1] = G_DHCPV6_IAADDR;
	buf[2] = 0;
	buf[3] = len;
	memcpy(&buf[4], &dhcp_client->ia_na, 16);
	memset(&buf[20], 0, 4); /* preferred */
	memset(&buf[24], 0, 4); /* valid */
	return buf;
}