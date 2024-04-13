static UINT rdpei_send_pdu(RDPEI_CHANNEL_CALLBACK* callback, wStream* s, UINT16 eventId,
                           UINT32 pduLength)
{
	UINT status;
	Stream_SetPosition(s, 0);
	Stream_Write_UINT16(s, eventId);   /* eventId (2 bytes) */
	Stream_Write_UINT32(s, pduLength); /* pduLength (4 bytes) */
	Stream_SetPosition(s, Stream_Length(s));
	status = callback->channel->Write(callback->channel, (UINT32)Stream_Length(s), Stream_Buffer(s),
	                                  NULL);
#ifdef WITH_DEBUG_RDPEI
	WLog_DBG(TAG,
	         "rdpei_send_pdu: eventId: %" PRIu16 " (%s) length: %" PRIu32 " status: %" PRIu32 "",
	         eventId, rdpei_eventid_string(eventId), pduLength, status);
#endif
	return status;
}