static gboolean start_renew(gpointer user_data)
{
	GDHCPClient *dhcp_client = user_data;

	debug(dhcp_client, "start renew");
	dhcp_client->state = RENEWING;

	/*calculate total renew period*/
	dhcp_client->T1 = dhcp_client->T2 - dhcp_client->T1;

	/*send first renew and reschedule for half the remaining time.*/
	continue_renew(user_data);

	return FALSE;
}