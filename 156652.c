static uint8_t *append_iaaddr(GDHCPClient *dhcp_client, uint8_t *buf,
			const char *address)
{
	struct in6_addr addr;

	if (inet_pton(AF_INET6, address, &addr) != 1)
		return NULL;

	buf[0] = 0;
	buf[1] = G_DHCPV6_IAADDR;
	buf[2] = 0;
	buf[3] = 24;
	memcpy(&buf[4], &addr, 16);
	memset(&buf[20], 0, 4); /* preferred */
	memset(&buf[24], 0, 4); /* valid */
	return &buf[28];
}