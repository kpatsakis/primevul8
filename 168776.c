static BOOL rdg_process_packet(rdpRdg* rdg, wStream* s)
{
	BOOL status = TRUE;
	UINT16 type;
	UINT32 packetLength;
	Stream_SetPosition(s, 0);

	if (Stream_GetRemainingLength(s) < 8)
	{
		WLog_ERR(TAG, "[%s] Short packet %" PRIuz ", expected 8", __FUNCTION__,
		         Stream_GetRemainingLength(s));
		return FALSE;
	}

	Stream_Read_UINT16(s, type);
	Stream_Seek_UINT16(s); /* reserved */
	Stream_Read_UINT32(s, packetLength);

	if (Stream_Length(s) < packetLength)
	{
		WLog_ERR(TAG, "[%s] Short packet %" PRIuz ", expected %" PRIuz, __FUNCTION__,
		         Stream_Length(s), packetLength);
		return FALSE;
	}

	switch (type)
	{
		case PKT_TYPE_HANDSHAKE_RESPONSE:
			status = rdg_process_handshake_response(rdg, s);
			break;

		case PKT_TYPE_TUNNEL_RESPONSE:
			status = rdg_process_tunnel_response(rdg, s);
			break;

		case PKT_TYPE_TUNNEL_AUTH_RESPONSE:
			status = rdg_process_tunnel_authorization_response(rdg, s);
			break;

		case PKT_TYPE_CHANNEL_RESPONSE:
			status = rdg_process_channel_response(rdg, s);
			break;

		case PKT_TYPE_DATA:
			WLog_ERR(TAG, "[%s] Unexpected packet type DATA", __FUNCTION__);
			return FALSE;
	}

	return status;
}