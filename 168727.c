static BOOL rdg_tls_connect(rdpRdg* rdg, rdpTls* tls, const char* peerAddress, int timeout)
{
	int sockfd = 0;
	long status = 0;
	BIO* socketBio = NULL;
	BIO* bufferedBio = NULL;
	rdpSettings* settings = rdg->settings;
	const char* peerHostname = settings->GatewayHostname;
	UINT16 peerPort = (UINT16)settings->GatewayPort;
	const char *proxyUsername, *proxyPassword;
	BOOL isProxyConnection =
	    proxy_prepare(settings, &peerHostname, &peerPort, &proxyUsername, &proxyPassword);

	if (settings->GatewayPort > UINT16_MAX)
		return FALSE;

	sockfd = freerdp_tcp_connect(rdg->context, settings, peerAddress ? peerAddress : peerHostname,
	                             peerPort, timeout);

	if (sockfd < 0)
	{
		return FALSE;
	}

	socketBio = BIO_new(BIO_s_simple_socket());

	if (!socketBio)
	{
		closesocket((SOCKET)sockfd);
		return FALSE;
	}

	BIO_set_fd(socketBio, sockfd, BIO_CLOSE);
	bufferedBio = BIO_new(BIO_s_buffered_socket());

	if (!bufferedBio)
	{
		BIO_free_all(socketBio);
		return FALSE;
	}

	bufferedBio = BIO_push(bufferedBio, socketBio);
	status = BIO_set_nonblock(bufferedBio, TRUE);

	if (isProxyConnection)
	{
		if (!proxy_connect(settings, bufferedBio, proxyUsername, proxyPassword,
		                   settings->GatewayHostname, (UINT16)settings->GatewayPort))
		{
			BIO_free_all(bufferedBio);
			return FALSE;
		}
	}

	if (!status)
	{
		BIO_free_all(bufferedBio);
		return FALSE;
	}

	tls->hostname = settings->GatewayHostname;
	tls->port = (int)settings->GatewayPort;
	tls->isGatewayTransport = TRUE;
	status = tls_connect(tls, bufferedBio);
	if (status < 1)
	{
		rdpContext* context = rdg->context;
		if (status < 0)
		{
			freerdp_set_last_error_if_not(context, FREERDP_ERROR_TLS_CONNECT_FAILED);
		}
		else
		{
			freerdp_set_last_error_if_not(context, FREERDP_ERROR_CONNECT_CANCELLED);
		}

		return FALSE;
	}
	return (status >= 1);
}