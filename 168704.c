BOOL nego_transport_connect(rdpNego* nego)
{
	if (!nego_tcp_connect(nego))
		return FALSE;

	if (nego->TcpConnected && !nego->NegotiateSecurityLayer)
		return nego_security_connect(nego);

	return nego->TcpConnected;
}