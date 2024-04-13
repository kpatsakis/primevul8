static UINT rdpei_send_cs_ready_pdu(RDPEI_CHANNEL_CALLBACK* callback)
{
	UINT status;
	wStream* s;
	UINT32 flags;
	UINT32 pduLength;
	RDPEI_PLUGIN* rdpei = (RDPEI_PLUGIN*)callback->plugin;
	flags = 0;
	flags |= READY_FLAGS_SHOW_TOUCH_VISUALS;
	// flags |= READY_FLAGS_DISABLE_TIMESTAMP_INJECTION;
	pduLength = RDPINPUT_HEADER_LENGTH + 10;
	s = Stream_New(NULL, pduLength);

	if (!s)
	{
		WLog_ERR(TAG, "Stream_New failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	Stream_Seek(s, RDPINPUT_HEADER_LENGTH);
	Stream_Write_UINT32(s, flags);                   /* flags (4 bytes) */
	Stream_Write_UINT32(s, RDPINPUT_PROTOCOL_V10);   /* protocolVersion (4 bytes) */
	Stream_Write_UINT16(s, rdpei->maxTouchContacts); /* maxTouchContacts (2 bytes) */
	Stream_SealLength(s);
	status = rdpei_send_pdu(callback, s, EVENTID_CS_READY, pduLength);
	Stream_Free(s, TRUE);
	return status;
}