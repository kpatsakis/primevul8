void nego_init(rdpNego* nego)
{
	nego->state = NEGO_STATE_INITIAL;
	nego->RequestedProtocols = PROTOCOL_RDP;
	nego->CookieMaxLength = DEFAULT_COOKIE_MAX_LENGTH;
	nego->sendNegoData = FALSE;
	nego->flags = 0;
}