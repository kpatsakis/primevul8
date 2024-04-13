int g_dhcp_client_start(GDHCPClient *dhcp_client, const char *last_address)
{
	int re;
	uint32_t addr;
	uint64_t rand;
	ClientState oldstate = dhcp_client->state;

	remove_timeouts(dhcp_client);

	if (dhcp_client->type == G_DHCP_IPV6) {
		if (dhcp_client->information_req_cb) {
			dhcp_client->state = INFORMATION_REQ;
			re = switch_listening_mode(dhcp_client, L3);
			if (re != 0) {
				switch_listening_mode(dhcp_client, L_NONE);
				dhcp_client->state = 0;
				return re;
			}
			send_information_req(dhcp_client);

		} else if (dhcp_client->solicitation_cb) {
			dhcp_client->state = SOLICITATION;
			re = switch_listening_mode(dhcp_client, L3);
			if (re != 0) {
				switch_listening_mode(dhcp_client, L_NONE);
				dhcp_client->state = 0;
				return re;
			}
			send_solicitation(dhcp_client);

		} else if (dhcp_client->request_cb) {
			dhcp_client->state = REQUEST;
			re = switch_listening_mode(dhcp_client, L3);
			if (re != 0) {
				switch_listening_mode(dhcp_client, L_NONE);
				dhcp_client->state = 0;
				return re;
			}
			send_dhcpv6_request(dhcp_client);

		} else if (dhcp_client->confirm_cb) {
			dhcp_client->state = CONFIRM;
			re = switch_listening_mode(dhcp_client, L3);
			if (re != 0) {
				switch_listening_mode(dhcp_client, L_NONE);
				dhcp_client->state = 0;
				return re;
			}
			send_dhcpv6_confirm(dhcp_client);

		} else if (dhcp_client->renew_cb) {
			dhcp_client->state = RENEW;
			re = switch_listening_mode(dhcp_client, L3);
			if (re != 0) {
				switch_listening_mode(dhcp_client, L_NONE);
				dhcp_client->state = 0;
				return re;
			}
			send_dhcpv6_renew(dhcp_client);

		} else if (dhcp_client->rebind_cb) {
			dhcp_client->state = REBIND;
			re = switch_listening_mode(dhcp_client, L3);
			if (re != 0) {
				switch_listening_mode(dhcp_client, L_NONE);
				dhcp_client->state = 0;
				return re;
			}
			send_dhcpv6_rebind(dhcp_client);

		} else if (dhcp_client->release_cb) {
			dhcp_client->state = RENEW;
			re = switch_listening_mode(dhcp_client, L3);
			if (re != 0) {
				switch_listening_mode(dhcp_client, L_NONE);
				dhcp_client->state = 0;
				return re;
			}
			send_dhcpv6_release(dhcp_client);
		} else if (dhcp_client->decline_cb) {
			dhcp_client->state = DECLINE;
			re = switch_listening_mode(dhcp_client, L3);
			if (re != 0) {
				switch_listening_mode(dhcp_client, L_NONE);
				dhcp_client->state = 0;
				return re;
			}
			send_dhcpv6_decline(dhcp_client);
		}

		return 0;
	}

	if (dhcp_client->type == G_DHCP_IPV4LL) {
		dhcp_client->state = INIT_SELECTING;
		ipv4ll_start(dhcp_client);
		return 0;
	}

	if (dhcp_client->retry_times == DISCOVER_RETRIES) {
		if (dhcp_client->no_lease_cb)
			dhcp_client->no_lease_cb(dhcp_client,
						dhcp_client->no_lease_data);
		dhcp_client->retry_times = 0;
		return 0;
	}

	if (dhcp_client->retry_times == 0) {
		g_free(dhcp_client->assigned_ip);
		dhcp_client->assigned_ip = NULL;

		dhcp_client->state = INIT_SELECTING;
		re = switch_listening_mode(dhcp_client, L2);
		if (re != 0)
			return re;

		__connman_util_get_random(&rand);
		dhcp_client->xid = rand;
		dhcp_client->start = time(NULL);
		g_hash_table_remove_all(dhcp_client->secs_bcast_hash);
	}

	if (!last_address || oldstate == DECLINED) {
		addr = 0;
	} else {
		addr = ntohl(inet_addr(last_address));
		if (addr == 0xFFFFFFFF || ((addr & LINKLOCAL_ADDR) ==
					LINKLOCAL_ADDR)) {
			addr = 0;
		} else if (dhcp_client->last_address != last_address) {
			g_free(dhcp_client->last_address);
			dhcp_client->last_address = g_strdup(last_address);
		}
	}

	if ((addr != 0) && (dhcp_client->type != G_DHCP_IPV4LL)) {
		debug(dhcp_client, "DHCP client start with state init_reboot");
		dhcp_client->requested_ip = addr;
		dhcp_client->state = REBOOTING;
		send_request(dhcp_client);

		dhcp_client->timeout = g_timeout_add_seconds_full(
								G_PRIORITY_HIGH,
								REQUEST_TIMEOUT,
								reboot_timeout,
								dhcp_client,
								NULL);
		return 0;
	}
	send_discover(dhcp_client, addr);

	dhcp_client->timeout = g_timeout_add_seconds_full(G_PRIORITY_HIGH,
							DISCOVER_TIMEOUT,
							discover_timeout,
							dhcp_client,
							NULL);
	return 0;
}