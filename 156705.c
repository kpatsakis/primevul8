int g_dhcpv6_client_get_timeouts(GDHCPClient *dhcp_client,
				uint32_t *T1, uint32_t *T2,
				time_t *started,
				time_t *expire)
{
	if (!dhcp_client || dhcp_client->type != G_DHCP_IPV6)
		return -EINVAL;

	if (T1)
		*T1 = (dhcp_client->expire == 0xffffffff) ? 0xffffffff:
			dhcp_client->T1;

	if (T2)
		*T2 = (dhcp_client->expire == 0xffffffff) ? 0xffffffff:
			dhcp_client->T2;

	if (started)
		*started = dhcp_client->last_request;

	if (expire)
		*expire = (dhcp_client->expire == 0xffffffff) ? 0xffffffff:
			dhcp_client->last_request + dhcp_client->expire;

	return 0;
}