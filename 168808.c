void nego_send(rdpNego* nego)
{
	if (nego->state == NEGO_STATE_EXT)
		nego_attempt_ext(nego);
	else if (nego->state == NEGO_STATE_NLA)
		nego_attempt_nla(nego);
	else if (nego->state == NEGO_STATE_TLS)
		nego_attempt_tls(nego);
	else if (nego->state == NEGO_STATE_RDP)
		nego_attempt_rdp(nego);
	else
		WLog_ERR(TAG, "invalid negotiation state for sending");
}