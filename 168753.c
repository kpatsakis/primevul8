static void nego_attempt_ext(rdpNego* nego)
{
	nego->RequestedProtocols = PROTOCOL_HYBRID | PROTOCOL_SSL | PROTOCOL_HYBRID_EX;
	WLog_DBG(TAG, "Attempting NLA extended security");

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

	WLog_DBG(TAG, "state: %s", nego_state_string(nego->state));

	if (nego->state != NEGO_STATE_FINAL)
	{
		nego_transport_disconnect(nego);

		if (nego->EnabledProtocols[PROTOCOL_HYBRID])
			nego->state = NEGO_STATE_NLA;
		else if (nego->EnabledProtocols[PROTOCOL_SSL])
			nego->state = NEGO_STATE_TLS;
		else if (nego->EnabledProtocols[PROTOCOL_RDP])
			nego->state = NEGO_STATE_RDP;
		else
			nego->state = NEGO_STATE_FAIL;
	}
}