rdpRdg* rdg_new(rdpContext* context)
{
	rdpRdg* rdg;
	RPC_CSTR stringUuid;
	char bracedUuid[40];
	RPC_STATUS rpcStatus;

	if (!context)
		return NULL;

	rdg = (rdpRdg*)calloc(1, sizeof(rdpRdg));

	if (rdg)
	{
		rdg->state = RDG_CLIENT_STATE_INITIAL;
		rdg->context = context;
		rdg->settings = rdg->context->settings;
		rdg->extAuth = HTTP_EXTENDED_AUTH_NONE;

		if (rdg->settings->GatewayAccessToken)
			rdg->extAuth = HTTP_EXTENDED_AUTH_PAA;

		UuidCreate(&rdg->guid);
		rpcStatus = UuidToStringA(&rdg->guid, &stringUuid);

		if (rpcStatus == RPC_S_OUT_OF_MEMORY)
			goto rdg_alloc_error;

		sprintf_s(bracedUuid, sizeof(bracedUuid), "{%s}", stringUuid);
		RpcStringFreeA(&stringUuid);
		rdg->tlsOut = tls_new(rdg->settings);

		if (!rdg->tlsOut)
			goto rdg_alloc_error;

		rdg->tlsIn = tls_new(rdg->settings);

		if (!rdg->tlsIn)
			goto rdg_alloc_error;

		rdg->http = http_context_new();

		if (!rdg->http)
			goto rdg_alloc_error;

		if (!http_context_set_uri(rdg->http, "/remoteDesktopGateway/") ||
		    !http_context_set_accept(rdg->http, "*/*") ||
		    !http_context_set_cache_control(rdg->http, "no-cache") ||
		    !http_context_set_pragma(rdg->http, "no-cache") ||
		    !http_context_set_connection(rdg->http, "Keep-Alive") ||
		    !http_context_set_user_agent(rdg->http, "MS-RDGateway/1.0") ||
		    !http_context_set_host(rdg->http, rdg->settings->GatewayHostname) ||
		    !http_context_set_rdg_connection_id(rdg->http, bracedUuid))
		{
			goto rdg_alloc_error;
		}

		if (rdg->extAuth != HTTP_EXTENDED_AUTH_NONE)
		{
			switch (rdg->extAuth)
			{
				case HTTP_EXTENDED_AUTH_PAA:
					if (!http_context_set_rdg_auth_scheme(rdg->http, "PAA"))
						goto rdg_alloc_error;

					break;

				default:
					WLog_DBG(TAG, "RDG extended authentication method %d not supported",
					         rdg->extAuth);
			}
		}

		rdg->frontBio = BIO_new(BIO_s_rdg());

		if (!rdg->frontBio)
			goto rdg_alloc_error;

		BIO_set_data(rdg->frontBio, rdg);
		InitializeCriticalSection(&rdg->writeSection);
	}

	return rdg;
rdg_alloc_error:
	rdg_free(rdg);
	return NULL;
}