const BYTE* nego_get_routing_token(rdpNego* nego, DWORD* RoutingTokenLength)
{
	if (!nego)
		return NULL;
	if (RoutingTokenLength)
		*RoutingTokenLength = nego->RoutingTokenLength;
	return nego->RoutingToken;
}