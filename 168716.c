BOOL nego_disconnect(rdpNego* nego)
{
	nego->state = NEGO_STATE_INITIAL;
	return nego_transport_disconnect(nego);
}