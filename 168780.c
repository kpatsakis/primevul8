BOOL nego_security_connect(rdpNego* nego)
{
	if (!nego->TcpConnected)
	{
		nego->SecurityConnected = FALSE;
	}
	else if (!nego->SecurityConnected)
	{
		if (nego->SelectedProtocol == PROTOCOL_HYBRID)
		{
			WLog_DBG(TAG, "nego_security_connect with PROTOCOL_HYBRID");
			nego->SecurityConnected = transport_connect_nla(nego->transport);
		}
		else if (nego->SelectedProtocol == PROTOCOL_SSL)
		{
			WLog_DBG(TAG, "nego_security_connect with PROTOCOL_SSL");
			nego->SecurityConnected = transport_connect_tls(nego->transport);
		}
		else if (nego->SelectedProtocol == PROTOCOL_RDP)
		{
			WLog_DBG(TAG, "nego_security_connect with PROTOCOL_RDP");
			nego->SecurityConnected = transport_connect_rdp(nego->transport);
		}
		else
		{
			WLog_ERR(TAG,
			         "cannot connect security layer because no protocol has been selected yet.");
		}
	}

	return nego->SecurityConnected;
}