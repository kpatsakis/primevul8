static void nego_attempt_rdp(rdpNego* nego)
{
	nego->RequestedProtocols = PROTOCOL_RDP;
	WLog_DBG(TAG, "Attempting RDP security");

	if (!nego_transport_connect(nego))
	{
		nego->state = NEGO_STATE_FAIL;
		return;
	}

	if (!nego_send_negotiation_request(nego))
	{
		nego->state = NEGO_STATE_FAIL;
		return;
	}

	if (!nego_recv_response(nego))
	{
		nego->state = NEGO_STATE_FAIL;
		return;
	}
}