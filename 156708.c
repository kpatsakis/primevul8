static void add_send_options(GDHCPClient *dhcp_client,
				struct dhcp_packet *packet)
{
	g_hash_table_foreach(dhcp_client->send_value_hash,
				add_binary_option, packet);
}