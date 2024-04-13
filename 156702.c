static void init_packet(GDHCPClient *dhcp_client, gpointer pkt, char type)
{
	if (dhcp_client->type == G_DHCP_IPV6)
		dhcpv6_init_header(pkt, type);
	else {
		struct dhcp_packet *packet = pkt;

		dhcp_init_header(packet, type);
		memcpy(packet->chaddr, dhcp_client->mac_address, 6);
	}
}