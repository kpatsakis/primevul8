static gboolean ipv4ll_probe_timeout(gpointer dhcp_data)
{

	GDHCPClient *dhcp_client = dhcp_data;

	debug(dhcp_client, "IPV4LL probe timeout (retries %d)",
	       dhcp_client->retry_times);

	if (dhcp_client->retry_times == PROBE_NUM) {
		dhcp_client->state = IPV4LL_ANNOUNCE;
		dhcp_client->retry_times = 0;

		dhcp_client->retry_times++;
		send_announce_packet(dhcp_client);
		return FALSE;
	}
	dhcp_client->retry_times++;
	send_probe_packet(dhcp_client);

	return FALSE;
}