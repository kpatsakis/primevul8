static BOOL rdg_process_handshake_response(rdpRdg* rdg, wStream* s)
{
	UINT32 errorCode;
	UINT16 serverVersion, extendedAuth;
	BYTE verMajor, verMinor;
	const char* error;
	WLog_DBG(TAG, "Handshake response received");

	if (rdg->state != RDG_CLIENT_STATE_HANDSHAKE)
	{
		return FALSE;
	}

	if (Stream_GetRemainingLength(s) < 10)
	{
		WLog_ERR(TAG, "[%s] Short packet %" PRIuz ", expected 10", __FUNCTION__,
		         Stream_GetRemainingLength(s));
		return FALSE;
	}

	Stream_Read_UINT32(s, errorCode);
	Stream_Read_UINT8(s, verMajor);
	Stream_Read_UINT8(s, verMinor);
	Stream_Read_UINT16(s, serverVersion);
	Stream_Read_UINT16(s, extendedAuth);
	error = rpc_error_to_string(errorCode);
	WLog_DBG(TAG,
	         "errorCode=%s, verMajor=%" PRId8 ", verMinor=%" PRId8 ", serverVersion=%" PRId16
	         ", extendedAuth=%s",
	         error, verMajor, verMinor, serverVersion, extended_auth_to_string(extendedAuth));

	if (FAILED(errorCode))
	{
		WLog_ERR(TAG, "Handshake error %s", error);
		freerdp_set_last_error_log(rdg->context, errorCode);
		return FALSE;
	}

	return rdg_send_tunnel_request(rdg);
}