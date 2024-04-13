void nego_free(rdpNego* nego)
{
	if (nego)
	{
		free(nego->RoutingToken);
		free(nego->cookie);
		free(nego);
	}
}