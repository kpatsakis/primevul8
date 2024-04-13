void g_dhcp_client_register_event(GDHCPClient *dhcp_client,
					GDHCPClientEvent event,
					GDHCPClientEventFunc func,
							gpointer data)
{
	switch (event) {
	case G_DHCP_CLIENT_EVENT_LEASE_AVAILABLE:
		dhcp_client->lease_available_cb = func;
		dhcp_client->lease_available_data = data;
		return;
	case G_DHCP_CLIENT_EVENT_IPV4LL_AVAILABLE:
		if (dhcp_client->type == G_DHCP_IPV6)
			return;
		dhcp_client->ipv4ll_available_cb = func;
		dhcp_client->ipv4ll_available_data = data;
		return;
	case G_DHCP_CLIENT_EVENT_NO_LEASE:
		dhcp_client->no_lease_cb = func;
		dhcp_client->no_lease_data = data;
		return;
	case G_DHCP_CLIENT_EVENT_LEASE_LOST:
		dhcp_client->lease_lost_cb = func;
		dhcp_client->lease_lost_data = data;
		return;
	case G_DHCP_CLIENT_EVENT_IPV4LL_LOST:
		if (dhcp_client->type == G_DHCP_IPV6)
			return;
		dhcp_client->ipv4ll_lost_cb = func;
		dhcp_client->ipv4ll_lost_data = data;
		return;
	case G_DHCP_CLIENT_EVENT_ADDRESS_CONFLICT:
		dhcp_client->address_conflict_cb = func;
		dhcp_client->address_conflict_data = data;
		return;
	case G_DHCP_CLIENT_EVENT_INFORMATION_REQ:
		if (dhcp_client->type != G_DHCP_IPV6)
			return;
		dhcp_client->information_req_cb = func;
		dhcp_client->information_req_data = data;
		return;
	case G_DHCP_CLIENT_EVENT_SOLICITATION:
		if (dhcp_client->type != G_DHCP_IPV6)
			return;
		dhcp_client->solicitation_cb = func;
		dhcp_client->solicitation_data = data;
		return;
	case G_DHCP_CLIENT_EVENT_ADVERTISE:
		if (dhcp_client->type != G_DHCP_IPV6)
			return;
		dhcp_client->advertise_cb = func;
		dhcp_client->advertise_data = data;
		return;
	case G_DHCP_CLIENT_EVENT_REQUEST:
		if (dhcp_client->type != G_DHCP_IPV6)
			return;
		dhcp_client->request_cb = func;
		dhcp_client->request_data = data;
		return;
	case G_DHCP_CLIENT_EVENT_RENEW:
		if (dhcp_client->type != G_DHCP_IPV6)
			return;
		dhcp_client->renew_cb = func;
		dhcp_client->renew_data = data;
		return;
	case G_DHCP_CLIENT_EVENT_REBIND:
		if (dhcp_client->type != G_DHCP_IPV6)
			return;
		dhcp_client->rebind_cb = func;
		dhcp_client->rebind_data = data;
		return;
	case G_DHCP_CLIENT_EVENT_RELEASE:
		if (dhcp_client->type != G_DHCP_IPV6)
			return;
		dhcp_client->release_cb = func;
		dhcp_client->release_data = data;
		return;
	case G_DHCP_CLIENT_EVENT_CONFIRM:
		if (dhcp_client->type != G_DHCP_IPV6)
			return;
		dhcp_client->confirm_cb = func;
		dhcp_client->confirm_data = data;
		return;
	case G_DHCP_CLIENT_EVENT_DECLINE:
		if (dhcp_client->type != G_DHCP_IPV6)
			return;
		dhcp_client->decline_cb = func;
		dhcp_client->decline_data = data;
		return;
	}
}