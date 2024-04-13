BOOL nego_send_negotiation_request(rdpNego* nego)
{
	BOOL rc = FALSE;
	wStream* s;
	size_t length;
	size_t bm, em;
	BYTE flags = 0;
	size_t cookie_length;
	s = Stream_New(NULL, 512);

	if (!s)
	{
		WLog_ERR(TAG, "Stream_New failed!");
		return FALSE;
	}

	length = TPDU_CONNECTION_REQUEST_LENGTH;
	bm = Stream_GetPosition(s);
	Stream_Seek(s, length);

	if (nego->RoutingToken)
	{
		Stream_Write(s, nego->RoutingToken, nego->RoutingTokenLength);

		/* Ensure Routing Token is correctly terminated - may already be present in string */

		if ((nego->RoutingTokenLength > 2) &&
		    (nego->RoutingToken[nego->RoutingTokenLength - 2] == 0x0D) &&
		    (nego->RoutingToken[nego->RoutingTokenLength - 1] == 0x0A))
		{
			WLog_DBG(TAG, "Routing token looks correctly terminated - use verbatim");
			length += nego->RoutingTokenLength;
		}
		else
		{
			WLog_DBG(TAG, "Adding terminating CRLF to routing token");
			Stream_Write_UINT8(s, 0x0D); /* CR */
			Stream_Write_UINT8(s, 0x0A); /* LF */
			length += nego->RoutingTokenLength + 2;
		}
	}
	else if (nego->cookie)
	{
		cookie_length = strlen(nego->cookie);

		if (cookie_length > nego->CookieMaxLength)
			cookie_length = nego->CookieMaxLength;

		Stream_Write(s, "Cookie: mstshash=", 17);
		Stream_Write(s, (BYTE*)nego->cookie, cookie_length);
		Stream_Write_UINT8(s, 0x0D); /* CR */
		Stream_Write_UINT8(s, 0x0A); /* LF */
		length += cookie_length + 19;
	}

	WLog_DBG(TAG, "RequestedProtocols: %" PRIu32 "", nego->RequestedProtocols);

	if ((nego->RequestedProtocols > PROTOCOL_RDP) || (nego->sendNegoData))
	{
		/* RDP_NEG_DATA must be present for TLS and NLA */
		if (nego->RestrictedAdminModeRequired)
			flags |= RESTRICTED_ADMIN_MODE_REQUIRED;

		Stream_Write_UINT8(s, TYPE_RDP_NEG_REQ);
		Stream_Write_UINT8(s, flags);
		Stream_Write_UINT16(s, 8);                        /* RDP_NEG_DATA length (8) */
		Stream_Write_UINT32(s, nego->RequestedProtocols); /* requestedProtocols */
		length += 8;
	}

	if (length > UINT16_MAX)
		goto fail;

	em = Stream_GetPosition(s);
	Stream_SetPosition(s, bm);
	tpkt_write_header(s, (UINT16)length);
	tpdu_write_connection_request(s, (UINT16)length - 5);
	Stream_SetPosition(s, em);
	Stream_SealLength(s);
	rc = (transport_write(nego->transport, s) >= 0);
fail:
	Stream_Free(s, TRUE);
	return rc;
}