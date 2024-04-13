static bool sanity_check(struct ip_udp_dhcp_packet *packet, int bytes)
{
	if (packet->ip.protocol != IPPROTO_UDP)
		return false;

	if (packet->ip.version != IPVERSION)
		return false;

	if (packet->ip.ihl != sizeof(packet->ip) >> 2)
		return false;

	if (packet->udp.dest != htons(CLIENT_PORT))
		return false;

	if (ntohs(packet->udp.len) != (uint16_t)(bytes - sizeof(packet->ip)))
		return false;

	return true;
}