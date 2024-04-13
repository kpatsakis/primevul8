static void start_request(GDHCPClient *dhcp_client)
{
	debug(dhcp_client, "start request (retries %d)",
					dhcp_client->retry_times);

	if (dhcp_client->retry_times == REQUEST_RETRIES) {
		if (dhcp_client->no_lease_cb)
			dhcp_client->no_lease_cb(dhcp_client,
						dhcp_client->no_lease_data);
		return;
	}

	if (dhcp_client->retry_times == 0) {
		dhcp_client->state = REQUESTING;
		switch_listening_mode(dhcp_client, L2);
	}

	send_request(dhcp_client);

	dhcp_client->timeout = g_timeout_add_seconds_full(G_PRIORITY_HIGH,
							REQUEST_TIMEOUT,
							request_timeout,
							dhcp_client,
							NULL);
}