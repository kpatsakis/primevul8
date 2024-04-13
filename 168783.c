static UINT rdpei_send_touch_event_pdu(RDPEI_CHANNEL_CALLBACK* callback,
                                       RDPINPUT_TOUCH_FRAME* frame)
{
	UINT status;
	wStream* s;
	UINT32 pduLength;
	pduLength = 64 + (frame->contactCount * 64);
	s = Stream_New(NULL, pduLength);

	if (!s)
	{
		WLog_ERR(TAG, "Stream_New failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	Stream_Seek(s, RDPINPUT_HEADER_LENGTH);
	/**
	 * the time that has elapsed (in milliseconds) from when the oldest touch frame
	 * was generated to when it was encoded for transmission by the client.
	 */
	rdpei_write_4byte_unsigned(
	    s, (UINT32)frame->frameOffset); /* encodeTime (FOUR_BYTE_UNSIGNED_INTEGER) */
	rdpei_write_2byte_unsigned(s, 1);   /* (frameCount) TWO_BYTE_UNSIGNED_INTEGER */

	if ((status = rdpei_write_touch_frame(s, frame)))
	{
		WLog_ERR(TAG, "rdpei_write_touch_frame failed with error %" PRIu32 "!", status);
		Stream_Free(s, TRUE);
		return status;
	}

	Stream_SealLength(s);
	pduLength = Stream_Length(s);
	status = rdpei_send_pdu(callback, s, EVENTID_TOUCH, pduLength);
	Stream_Free(s, TRUE);
	return status;
}