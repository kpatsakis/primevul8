static void nego_attempt_tls(rdpNego* nego)
{
	nego->RequestedProtocols = PROTOCOL_SSL;
	WLog_DBG(TAG, "Attempting TLS security");

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

	if (nego->state != NEGO_STATE_FINAL)
	{
		nego_transport_disconnect(nego);

		if (nego->EnabledProtocols[PROTOCOL_RDP])
			nego->state = NEGO_STATE_RDP;
		else
			nego->state = NEGO_STATE_FAIL;
	}
}