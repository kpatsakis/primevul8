BOOL nego_send_negotiation_response(rdpNego* nego)
{
	UINT16 length;
	size_t bm, em;
	BOOL status;
	wStream* s;
	BYTE flags;
	rdpSettings* settings;
	status = TRUE;
	settings = nego->transport->settings;
	s = Stream_New(NULL, 512);

	if (!s)
	{
		WLog_ERR(TAG, "Stream_New failed!");
		return FALSE;
	}

	length = TPDU_CONNECTION_CONFIRM_LENGTH;
	bm = Stream_GetPosition(s);
	Stream_Seek(s, length);

	if (nego->SelectedProtocol & PROTOCOL_FAILED_NEGO)
	{
		UINT32 errorCode = (nego->SelectedProtocol & ~PROTOCOL_FAILED_NEGO);
		flags = 0;
		Stream_Write_UINT8(s, TYPE_RDP_NEG_FAILURE);
		Stream_Write_UINT8(s, flags); /* flags */
		Stream_Write_UINT16(s, 8);    /* RDP_NEG_DATA length (8) */
		Stream_Write_UINT32(s, errorCode);
		length += 8;
		status = FALSE;
	}
	else
	{
		flags = EXTENDED_CLIENT_DATA_SUPPORTED;

		if (settings->SupportGraphicsPipeline)
			flags |= DYNVC_GFX_PROTOCOL_SUPPORTED;

		/* RDP_NEG_DATA must be present for TLS, NLA, and RDP */
		Stream_Write_UINT8(s, TYPE_RDP_NEG_RSP);
		Stream_Write_UINT8(s, flags);                   /* flags */
		Stream_Write_UINT16(s, 8);                      /* RDP_NEG_DATA length (8) */
		Stream_Write_UINT32(s, nego->SelectedProtocol); /* selectedProtocol */
		length += 8;
	}

	em = Stream_GetPosition(s);
	Stream_SetPosition(s, bm);
	tpkt_write_header(s, length);
	tpdu_write_connection_confirm(s, length - 5);
	Stream_SetPosition(s, em);
	Stream_SealLength(s);

	if (transport_write(nego->transport, s) < 0)
	{
		Stream_Free(s, TRUE);
		return FALSE;
	}

	Stream_Free(s, TRUE);

	if (status)
	{
		/* update settings with negotiated protocol security */
		settings->RequestedProtocols = nego->RequestedProtocols;
		settings->SelectedProtocol = nego->SelectedProtocol;

		if (settings->SelectedProtocol == PROTOCOL_RDP)
		{
			settings->TlsSecurity = FALSE;
			settings->NlaSecurity = FALSE;
			settings->RdpSecurity = TRUE;
			settings->UseRdpSecurityLayer = TRUE;

			if (settings->EncryptionLevel == ENCRYPTION_LEVEL_NONE)
			{
				/**
				 * If the server implementation did not explicitely set a
				 * encryption level we default to client compatible
				 */
				settings->EncryptionLevel = ENCRYPTION_LEVEL_CLIENT_COMPATIBLE;
			}

			if (settings->LocalConnection)
			{
				/**
				 * Note: This hack was firstly introduced in commit 95f5e115 to
				 * disable the unnecessary encryption with peers connecting to
				 * 127.0.0.1 or local unix sockets.
				 * This also affects connections via port tunnels! (e.g. ssh -L)
				 */
				WLog_INFO(TAG, "Turning off encryption for local peer with standard rdp security");
				settings->UseRdpSecurityLayer = FALSE;
				settings->EncryptionLevel = ENCRYPTION_LEVEL_NONE;
			}

			if (!settings->RdpServerRsaKey && !settings->RdpKeyFile && !settings->RdpKeyContent)
			{
				WLog_ERR(TAG, "Missing server certificate");
				return FALSE;
			}
		}
		else if (settings->SelectedProtocol == PROTOCOL_SSL)
		{
			settings->TlsSecurity = TRUE;
			settings->NlaSecurity = FALSE;
			settings->RdpSecurity = FALSE;
			settings->UseRdpSecurityLayer = FALSE;
			settings->EncryptionLevel = ENCRYPTION_LEVEL_NONE;
		}
		else if (settings->SelectedProtocol == PROTOCOL_HYBRID)
		{
			settings->TlsSecurity = TRUE;
			settings->NlaSecurity = TRUE;
			settings->RdpSecurity = FALSE;
			settings->UseRdpSecurityLayer = FALSE;
			settings->EncryptionLevel = ENCRYPTION_LEVEL_NONE;
		}
	}

	return status;
}