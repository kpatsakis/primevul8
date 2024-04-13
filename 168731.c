void nego_enable_rdp(rdpNego* nego, BOOL enable_rdp)
{
	WLog_DBG(TAG, "Enabling RDP security: %s", enable_rdp ? "TRUE" : "FALSE");
	nego->EnabledProtocols[PROTOCOL_RDP] = enable_rdp;
}