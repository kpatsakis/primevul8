static BOOL rdg_send_tunnel_authorization(rdpRdg* rdg)
{
	wStream* s;
	BOOL status;
	WCHAR* clientName = NULL;
	UINT32 packetSize;
	int clientNameLen =
	    ConvertToUnicode(CP_UTF8, 0, rdg->settings->ClientHostname, -1, &clientName, 0);

	if (!clientName || (clientNameLen < 0) || (clientNameLen > UINT16_MAX / 2))
	{
		free(clientName);
		return FALSE;
	}

	packetSize = 12 + (UINT32)clientNameLen * sizeof(WCHAR);
	s = Stream_New(NULL, packetSize);

	if (!s)
	{
		free(clientName);
		return FALSE;
	}

	Stream_Write_UINT16(s, PKT_TYPE_TUNNEL_AUTH);      /* Type (2 bytes) */
	Stream_Write_UINT16(s, 0);                         /* Reserved (2 bytes) */
	Stream_Write_UINT32(s, packetSize);                /* PacketLength (4 bytes) */
	Stream_Write_UINT16(s, 0);                         /* FieldsPresent (2 bytes) */
	Stream_Write_UINT16(s, (UINT16)clientNameLen * 2); /* Client name string length */
	Stream_Write_UTF16_String(s, clientName, (size_t)clientNameLen);
	Stream_SealLength(s);
	status = rdg_write_packet(rdg, s);
	Stream_Free(s, TRUE);
	free(clientName);

	if (status)
	{
		rdg->state = RDG_CLIENT_STATE_TUNNEL_AUTHORIZE;
	}

	return status;
}