static BOOL nego_tcp_connect(rdpNego* nego)
{
	if (!nego->TcpConnected)
	{
		if (nego->GatewayEnabled)
		{
			if (nego->GatewayBypassLocal)
			{
				/* Attempt a direct connection first, and then fallback to using the gateway */
				WLog_INFO(TAG,
				          "Detecting if host can be reached locally. - This might take some time.");
				WLog_INFO(TAG, "To disable auto detection use /gateway-usage-method:direct");
				transport_set_gateway_enabled(nego->transport, FALSE);
				nego->TcpConnected =
				    transport_connect(nego->transport, nego->hostname, nego->port, 1);
			}

			if (!nego->TcpConnected)
			{
				transport_set_gateway_enabled(nego->transport, TRUE);
				nego->TcpConnected =
				    transport_connect(nego->transport, nego->hostname, nego->port, 15);
			}
		}
		else
		{
			nego->TcpConnected = transport_connect(nego->transport, nego->hostname, nego->port, 15);
		}
	}

	return nego->TcpConnected;
}