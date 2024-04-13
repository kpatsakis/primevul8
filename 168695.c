BOOL nego_set_routing_token(rdpNego* nego, BYTE* RoutingToken, DWORD RoutingTokenLength)
{
	if (RoutingTokenLength == 0)
		return FALSE;

	free(nego->RoutingToken);
	nego->RoutingTokenLength = RoutingTokenLength;
	nego->RoutingToken = (BYTE*)malloc(nego->RoutingTokenLength);

	if (!nego->RoutingToken)
		return FALSE;

	CopyMemory(nego->RoutingToken, RoutingToken, nego->RoutingTokenLength);
	return TRUE;
}