BOOL nego_send_preconnection_pdu(rdpNego* nego)
{
	wStream* s;
	UINT32 cbSize;
	UINT16 cchPCB = 0;
	WCHAR* wszPCB = NULL;
	WLog_DBG(TAG, "Sending preconnection PDU");

	if (!nego_tcp_connect(nego))
		return FALSE;

	/* it's easier to always send the version 2 PDU, and it's just 2 bytes overhead */
	cbSize = PRECONNECTION_PDU_V2_MIN_SIZE;

	if (nego->PreconnectionBlob)
	{
		cchPCB = (UINT16)ConvertToUnicode(CP_UTF8, 0, nego->PreconnectionBlob, -1, &wszPCB, 0);
		cchPCB += 1; /* zero-termination */
		cbSize += cchPCB * 2;
	}

	s = Stream_New(NULL, cbSize);

	if (!s)
	{
		free(wszPCB);
		WLog_ERR(TAG, "Stream_New failed!");
		return FALSE;
	}

	Stream_Write_UINT32(s, cbSize);                /* cbSize */
	Stream_Write_UINT32(s, 0);                     /* Flags */
	Stream_Write_UINT32(s, PRECONNECTION_PDU_V2);  /* Version */
	Stream_Write_UINT32(s, nego->PreconnectionId); /* Id */
	Stream_Write_UINT16(s, cchPCB);                /* cchPCB */

	if (wszPCB)
	{
		Stream_Write(s, wszPCB, cchPCB * 2); /* wszPCB */
		free(wszPCB);
	}

	Stream_SealLength(s);

	if (transport_write(nego->transport, s) < 0)
	{
		Stream_Free(s, TRUE);
		return FALSE;
	}

	Stream_Free(s, TRUE);
	return TRUE;
}