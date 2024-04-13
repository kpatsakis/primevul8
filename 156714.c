gboolean g_dhcpv6_client_clear_send(GDHCPClient *dhcp_client, uint16_t code)
{
	return g_hash_table_remove(dhcp_client->send_value_hash,
				GINT_TO_POINTER((int)code));
}