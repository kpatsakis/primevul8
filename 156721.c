static gboolean ipv4ll_announce_timeout(gpointer dhcp_data)
{
	GDHCPClient *dhcp_client = dhcp_data;
	uint32_t ip;

	debug(dhcp_client, "request timeout (retries %d)",
	       dhcp_client->retry_times);

	if (dhcp_client->retry_times != ANNOUNCE_NUM) {
		dhcp_client->retry_times++;
		send_announce_packet(dhcp_client);
		return FALSE;
	}

	ip = htonl(dhcp_client->requested_ip);
	debug(dhcp_client, "switching to monitor mode");
	dhcp_client->state = IPV4LL_MONITOR;
	dhcp_client->assigned_ip = get_ip(ip);

	if (dhcp_client->ipv4ll_available_cb)
		dhcp_client->ipv4ll_available_cb(dhcp_client,
					dhcp_client->ipv4ll_available_data);
	dhcp_client->conflicts = 0;
	dhcp_client->timeout = 0;

	return FALSE;
}