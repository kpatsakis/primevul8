static int switch_listening_mode(GDHCPClient *dhcp_client,
					ListenMode listen_mode)
{
	GIOChannel *listener_channel;
	int listener_sockfd;

	if (dhcp_client->listen_mode == listen_mode)
		return 0;

	debug(dhcp_client, "switch listening mode (%d ==> %d)",
				dhcp_client->listen_mode, listen_mode);

	if (dhcp_client->listen_mode != L_NONE) {
		if (dhcp_client->listener_watch > 0)
			g_source_remove(dhcp_client->listener_watch);
		dhcp_client->listen_mode = L_NONE;
		dhcp_client->listener_sockfd = -1;
		dhcp_client->listener_watch = 0;
	}

	if (listen_mode == L_NONE)
		return 0;

	if (listen_mode == L2)
		listener_sockfd = dhcp_l2_socket(dhcp_client->ifindex);
	else if (listen_mode == L3) {
		if (dhcp_client->type == G_DHCP_IPV6)
			listener_sockfd = dhcp_l3_socket(DHCPV6_CLIENT_PORT,
							dhcp_client->interface,
							AF_INET6);
		else
			listener_sockfd = dhcp_l3_socket(CLIENT_PORT,
							dhcp_client->interface,
							AF_INET);
	} else if (listen_mode == L_ARP)
		listener_sockfd = arp_socket(dhcp_client->ifindex);
	else
		return -EIO;

	if (listener_sockfd < 0)
		return -EIO;

	listener_channel = g_io_channel_unix_new(listener_sockfd);
	if (!listener_channel) {
		/* Failed to create listener channel */
		close(listener_sockfd);
		return -EIO;
	}

	dhcp_client->listen_mode = listen_mode;
	dhcp_client->listener_sockfd = listener_sockfd;

	g_io_channel_set_close_on_unref(listener_channel, TRUE);
	dhcp_client->listener_watch =
			g_io_add_watch_full(listener_channel, G_PRIORITY_HIGH,
				G_IO_IN | G_IO_NVAL | G_IO_ERR | G_IO_HUP,
						listener_event, g_dhcp_client_ref(dhcp_client),
								listener_watch_destroy);
	g_io_channel_unref(listener_channel);

	return 0;
}