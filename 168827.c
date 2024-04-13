static BOOL nego_read_request_token_or_cookie(rdpNego* nego, wStream* s)
{
	/* routingToken and cookie are optional and mutually exclusive!
	 *
	 * routingToken (variable): An optional and variable-length routing
	 * token (used for load balancing) terminated by a 0x0D0A two-byte
	 * sequence: (check [MSFT-SDLBTS] for details!)
	 * Cookie:[space]msts=[ip address].[port].[reserved][\x0D\x0A]
	 *
	 * cookie (variable): An optional and variable-length ANSI character
	 * string terminated by a 0x0D0A two-byte sequence:
	 * Cookie:[space]mstshash=[ANSISTRING][\x0D\x0A]
	 */
	BYTE* str = NULL;
	UINT16 crlf = 0;
	size_t pos, len;
	BOOL result = FALSE;
	BOOL isToken = FALSE;
	size_t remain = Stream_GetRemainingLength(s);
	str = Stream_Pointer(s);
	pos = Stream_GetPosition(s);

	/* minimum length for token is 15 */
	if (remain < 15)
		return TRUE;

	if (memcmp(Stream_Pointer(s), "Cookie: mstshash=", 17) != 0)
	{
		isToken = TRUE;
	}
	else
	{
		/* not a token, minimum length for cookie is 19 */
		if (remain < 19)
			return TRUE;

		Stream_Seek(s, 17);
	}

	while ((remain = Stream_GetRemainingLength(s)) >= 2)
	{
		Stream_Read_UINT16(s, crlf);

		if (crlf == 0x0A0D)
			break;

		Stream_Rewind(s, 1);
	}

	if (crlf == 0x0A0D)
	{
		Stream_Rewind(s, 2);
		len = Stream_GetPosition(s) - pos;
		remain = Stream_GetRemainingLength(s);
		Stream_Write_UINT16(s, 0);

		if (strnlen((char*)str, len) == len)
		{
			if (isToken)
				result = nego_set_routing_token(nego, str, len);
			else
				result = nego_set_cookie(nego, (char*)str);
		}
	}

	if (!result)
	{
		Stream_SetPosition(s, pos);
		WLog_ERR(TAG, "invalid %s received", isToken ? "routing token" : "cookie");
	}
	else
	{
		WLog_DBG(TAG, "received %s [%s]", isToken ? "routing token" : "cookie", str);
	}

	return result;
}