static int ipv4ll_recv_arp_packet(GDHCPClient *dhcp_client)
{
	int bytes;
	struct ether_arp arp;
	uint32_t ip_requested;
	int source_conflict;
	int target_conflict;
	guint timeout_ms;

	memset(&arp, 0, sizeof(arp));
	bytes = read(dhcp_client->listener_sockfd, &arp, sizeof(arp));
	if (bytes < 0)
		return bytes;

	if (arp.arp_op != htons(ARPOP_REPLY) &&
			arp.arp_op != htons(ARPOP_REQUEST))
		return -EINVAL;

	if (memcmp(arp.arp_sha, dhcp_client->mac_address, ETH_ALEN) == 0)
		return 0;

	ip_requested = htonl(dhcp_client->requested_ip);
	source_conflict = !memcmp(arp.arp_spa, &ip_requested,
						sizeof(ip_requested));

	target_conflict = !memcmp(arp.arp_tpa, &ip_requested,
				sizeof(ip_requested));

	if (!source_conflict && !target_conflict)
		return 0;

	dhcp_client->conflicts++;

	debug(dhcp_client, "IPV4LL conflict detected");

	if (dhcp_client->state == IPV4LL_MONITOR) {
		if (!source_conflict)
			return 0;
		dhcp_client->state = IPV4LL_DEFEND;
		debug(dhcp_client, "DEFEND mode conflicts : %d",
			dhcp_client->conflicts);
		/*Try to defend with a single announce*/
		send_announce_packet(dhcp_client);
		return 0;
	}

	if (dhcp_client->state == IPV4LL_DEFEND) {
		if (!source_conflict)
			return 0;
		else if (dhcp_client->ipv4ll_lost_cb)
			dhcp_client->ipv4ll_lost_cb(dhcp_client,
						dhcp_client->ipv4ll_lost_data);
	}

	ipv4ll_stop(dhcp_client);

	/* If we got a lot of conflicts, RFC3927 states that we have
	 * to wait RATE_LIMIT_INTERVAL before retrying,
	 */
	if (dhcp_client->conflicts < MAX_CONFLICTS)
		timeout_ms = __connman_util_random_delay_ms(PROBE_WAIT);
	else
		timeout_ms = RATE_LIMIT_INTERVAL * 1000;
	dhcp_client->retry_times++;
	dhcp_client->timeout =
		g_timeout_add_full(G_PRIORITY_HIGH,
				timeout_ms,
				send_probe_packet,
				dhcp_client,
				NULL);

	return 0;
}