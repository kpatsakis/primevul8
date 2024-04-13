void nego_set_negotiation_enabled(rdpNego* nego, BOOL NegotiateSecurityLayer)
{
	WLog_DBG(TAG, "Enabling security layer negotiation: %s",
	         NegotiateSecurityLayer ? "TRUE" : "FALSE");
	nego->NegotiateSecurityLayer = NegotiateSecurityLayer;
}