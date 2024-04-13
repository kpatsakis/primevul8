BOOL nego_set_target(rdpNego* nego, const char* hostname, UINT16 port)
{
	if (!nego || !hostname)
		return FALSE;

	nego->hostname = hostname;
	nego->port = port;
	return TRUE;
}