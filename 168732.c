BOOL rdg_connect(rdpRdg* rdg, int timeout, BOOL* rpcFallback)
{
	BOOL status;
	SOCKET outConnSocket = 0;
	char* peerAddress = NULL;
	assert(rdg != NULL);
	status =
	    rdg_establish_data_connection(rdg, rdg->tlsOut, "RDG_OUT_DATA", NULL, timeout, rpcFallback);

	if (status)
	{
		/* Establish IN connection with the same peer/server as OUT connection,
		 * even when server hostname resolves to different IP addresses.
		 */
		BIO_get_socket(rdg->tlsOut->underlying, &outConnSocket);
		peerAddress = freerdp_tcp_get_peer_address(outConnSocket);
		status = rdg_establish_data_connection(rdg, rdg->tlsIn, "RDG_IN_DATA", peerAddress, timeout,
		                                       NULL);
		free(peerAddress);
	}

	if (!status)
	{
		rdg->context->rdp->transport->layer = TRANSPORT_LAYER_CLOSED;
		return FALSE;
	}

	status = rdg_tunnel_connect(rdg);

	if (!status)
		return FALSE;

	return TRUE;
}