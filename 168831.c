static BOOL rdg_establish_data_connection(rdpRdg* rdg, rdpTls* tls, const char* method,
                                          const char* peerAddress, int timeout, BOOL* rpcFallback)
{
	HttpResponse* response = NULL;
	long statusCode;
	SSIZE_T bodyLength;
	long StatusCode;

	if (!rdg_tls_connect(rdg, tls, peerAddress, timeout))
		return FALSE;

	if (rdg->extAuth == HTTP_EXTENDED_AUTH_NONE)
	{
		if (!rdg_ntlm_init(rdg, tls))
			return FALSE;

		if (!rdg_send_http_request(rdg, tls, method, NULL))
			return FALSE;

		response = http_response_recv(tls, TRUE);

		if (!response)
			return FALSE;

		StatusCode = http_response_get_status_code(response);

		switch (StatusCode)
		{
			case HTTP_STATUS_NOT_FOUND:
			{
				WLog_INFO(TAG, "RD Gateway does not support HTTP transport.");

				if (rpcFallback)
					*rpcFallback = TRUE;

				http_response_free(response);
				return FALSE;
			}
			default:
				break;
		}

		if (!rdg_handle_ntlm_challenge(rdg->ntlm, response))
		{
			http_response_free(response);
			return FALSE;
		}

		http_response_free(response);
	}

	if (!rdg_send_http_request(rdg, tls, method, NULL))
		return FALSE;

	ntlm_free(rdg->ntlm);
	rdg->ntlm = NULL;
	response = http_response_recv(tls, TRUE);

	if (!response)
		return FALSE;

	statusCode = http_response_get_status_code(response);
	bodyLength = http_response_get_body_length(response);
	http_response_free(response);
	WLog_DBG(TAG, "%s authorization result: %d", method, statusCode);

	switch (statusCode)
	{
		case HTTP_STATUS_OK:
			break;
		case HTTP_STATUS_DENIED:
			freerdp_set_last_error_log(rdg->context, FREERDP_ERROR_CONNECT_ACCESS_DENIED);
			return FALSE;
		default:
			return FALSE;
	}

	if (strcmp(method, "RDG_OUT_DATA") == 0)
	{
		if (!rdg_skip_seed_payload(tls, bodyLength))
			return FALSE;
	}
	else
	{
		if (!rdg_send_http_request(rdg, tls, method, "chunked"))
			return FALSE;
	}

	return TRUE;
}