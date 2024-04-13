static gboolean restart_dhcp_timeout(gpointer user_data)
{
	GDHCPClient *dhcp_client = user_data;

	debug(dhcp_client, "restart DHCP timeout");

	if (dhcp_client->state == REBOOTING) {
		g_free(dhcp_client->last_address);
		dhcp_client->last_address = NULL;
		restart_dhcp(dhcp_client, 0);
	} else {
		dhcp_client->ack_retry_times++;
		restart_dhcp(dhcp_client, dhcp_client->ack_retry_times);
	}
	return FALSE;
}