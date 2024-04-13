void g_dhcp_client_stop(GDHCPClient *dhcp_client)
{
	switch_listening_mode(dhcp_client, L_NONE);

	if (dhcp_client->state == BOUND ||
			dhcp_client->state == RENEWING ||
				dhcp_client->state == REBINDING)
		send_release(dhcp_client, dhcp_client->server_ip,
					dhcp_client->requested_ip);

	remove_timeouts(dhcp_client);

	if (dhcp_client->listener_watch > 0) {
		g_source_remove(dhcp_client->listener_watch);
		dhcp_client->listener_watch = 0;
	}

	dhcp_client->retry_times = 0;
	dhcp_client->ack_retry_times = 0;

	dhcp_client->requested_ip = 0;
	dhcp_client->state = RELEASED;
	dhcp_client->lease_seconds = 0;
	dhcp_client->request_bcast = false;
}