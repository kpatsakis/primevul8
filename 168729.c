UINT32 nego_get_requested_protocols(rdpNego* nego)
{
	if (!nego)
		return 0;

	return nego->RequestedProtocols;
}