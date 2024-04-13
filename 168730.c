BOOL nego_transport_disconnect(rdpNego* nego)
{
	if (nego->TcpConnected)
		transport_disconnect(nego->transport);

	nego->TcpConnected = FALSE;
	nego->SecurityConnected = FALSE;
	return TRUE;
}