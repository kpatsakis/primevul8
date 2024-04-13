static BOOL rdg_handle_ntlm_challenge(rdpNtlm* ntlm, HttpResponse* response)
{
	BOOL continueNeeded = FALSE;
	size_t len;
	const char* token64 = NULL;
	int ntlmTokenLength = 0;
	BYTE* ntlmTokenData = NULL;
	long StatusCode;

	if (!ntlm || !response)
		return FALSE;

	StatusCode = http_response_get_status_code(response);

	if (StatusCode != HTTP_STATUS_DENIED)
	{
		WLog_DBG(TAG, "Unexpected NTLM challenge HTTP status: %ld", StatusCode);
		return FALSE;
	}

	token64 = http_response_get_auth_token(response, "NTLM");

	if (!token64)
		return FALSE;

	len = strlen(token64);

	if (len > INT_MAX)
		return FALSE;

	crypto_base64_decode(token64, (int)len, &ntlmTokenData, &ntlmTokenLength);

	if (ntlmTokenLength < 0)
	{
		free(ntlmTokenData);
		return FALSE;
	}

	if (ntlmTokenData && ntlmTokenLength)
	{
		if (!ntlm_client_set_input_buffer(ntlm, FALSE, ntlmTokenData, (size_t)ntlmTokenLength))
			return FALSE;
	}

	if (!ntlm_authenticate(ntlm, &continueNeeded))
		return FALSE;

	if (continueNeeded)
		return FALSE;

	return TRUE;
}