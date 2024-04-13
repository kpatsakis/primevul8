static uint16_t dhcp_attempt_secs(GDHCPClient *dhcp_client)
{
	return htons(MIN(time(NULL) - dhcp_client->start, UINT16_MAX));
}