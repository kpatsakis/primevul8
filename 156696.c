GDHCPClient *g_dhcp_client_new(GDHCPType type,
			int ifindex, GDHCPClientError *error)
{
	GDHCPClient *dhcp_client;

	if (ifindex < 0) {
		*error = G_DHCP_CLIENT_ERROR_INVALID_INDEX;
		return NULL;
	}

	dhcp_client = g_try_new0(GDHCPClient, 1);
	if (!dhcp_client) {
		*error = G_DHCP_CLIENT_ERROR_NOMEM;
		return NULL;
	}

	dhcp_client->interface = get_interface_name(ifindex);
	if (!dhcp_client->interface) {
		*error = G_DHCP_CLIENT_ERROR_INTERFACE_UNAVAILABLE;
		goto error;
	}

	if (!interface_is_up(ifindex)) {
		*error = G_DHCP_CLIENT_ERROR_INTERFACE_DOWN;
		goto error;
	}

	__connman_inet_get_interface_mac_address(ifindex, dhcp_client->mac_address);

	dhcp_client->listener_sockfd = -1;
	dhcp_client->listen_mode = L_NONE;
	dhcp_client->ref_count = 1;
	dhcp_client->type = type;
	dhcp_client->ifindex = ifindex;
	dhcp_client->lease_available_cb = NULL;
	dhcp_client->ipv4ll_available_cb = NULL;
	dhcp_client->no_lease_cb = NULL;
	dhcp_client->lease_lost_cb = NULL;
	dhcp_client->ipv4ll_lost_cb = NULL;
	dhcp_client->address_conflict_cb = NULL;
	dhcp_client->listener_watch = 0;
	dhcp_client->retry_times = 0;
	dhcp_client->ack_retry_times = 0;
	dhcp_client->code_value_hash = g_hash_table_new_full(g_direct_hash,
				g_direct_equal, NULL, remove_option_value);
	dhcp_client->send_value_hash = g_hash_table_new_full(g_direct_hash,
				g_direct_equal, NULL, g_free);
	dhcp_client->secs_bcast_hash = g_hash_table_new(g_direct_hash,
				g_direct_equal);
	dhcp_client->request_list = NULL;
	dhcp_client->require_list = NULL;
	dhcp_client->duid = NULL;
	dhcp_client->duid_len = 0;
	dhcp_client->last_request = time(NULL);
	dhcp_client->expire = 0;
	dhcp_client->request_bcast = false;

	*error = G_DHCP_CLIENT_ERROR_NONE;

	return dhcp_client;

error:
	g_free(dhcp_client->interface);
	g_free(dhcp_client);
	return NULL;
}