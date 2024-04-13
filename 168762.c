static BOOL rdg_set_ntlm_auth_header(rdpNtlm* ntlm, HttpRequest* request)
{
	const SecBuffer* ntlmToken = ntlm_client_get_output_buffer(ntlm);
	char* base64NtlmToken = NULL;

	if (ntlmToken)
	{
		if (ntlmToken->cbBuffer > INT_MAX)
			return FALSE;

		base64NtlmToken = crypto_base64_encode(ntlmToken->pvBuffer, (int)ntlmToken->cbBuffer);
	}

	if (base64NtlmToken)
	{
		BOOL rc = http_request_set_auth_scheme(request, "NTLM") &&
		          http_request_set_auth_param(request, base64NtlmToken);
		free(base64NtlmToken);

		if (!rc)
			return FALSE;
	}

	return TRUE;
}