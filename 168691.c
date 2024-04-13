BOOL nego_set_requested_protocols(rdpNego* nego, UINT32 RequestedProtocols)
{
	if (!nego)
		return FALSE;

	nego->RequestedProtocols = RequestedProtocols;
	return TRUE;
}