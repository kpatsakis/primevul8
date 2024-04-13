void nego_enable_nla(rdpNego* nego, BOOL enable_nla)
{
	WLog_DBG(TAG, "Enabling NLA security: %s", enable_nla ? "TRUE" : "FALSE");
	nego->EnabledProtocols[PROTOCOL_HYBRID] = enable_nla;
}