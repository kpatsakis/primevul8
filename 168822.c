BOOL nego_recv_response(rdpNego* nego)
{
	int status;
	wStream* s;
	s = Stream_New(NULL, 1024);

	if (!s)
	{
		WLog_ERR(TAG, "Stream_New failed!");
		return FALSE;
	}

	status = transport_read_pdu(nego->transport, s);

	if (status < 0)
	{
		Stream_Free(s, TRUE);
		return FALSE;
	}

	status = nego_recv(nego->transport, s, nego);
	Stream_Free(s, TRUE);

	if (status < 0)
		return FALSE;

	return TRUE;
}