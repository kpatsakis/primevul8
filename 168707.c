static BOOL rdg_process_tunnel_response(rdpRdg* rdg, wStream* s)
{
	UINT16 serverVersion, fieldsPresent;
	UINT32 errorCode;
	const char* error;
	WLog_DBG(TAG, "Tunnel response received");

	if (rdg->state != RDG_CLIENT_STATE_TUNNEL_CREATE)
	{
		return FALSE;
	}

	if (Stream_GetRemainingLength(s) < 10)
	{
		WLog_ERR(TAG, "[%s] Short packet %" PRIuz ", expected 10", __FUNCTION__,
		         Stream_GetRemainingLength(s));
		return FALSE;
	}

	Stream_Read_UINT16(s, serverVersion);
	Stream_Read_UINT32(s, errorCode);
	Stream_Read_UINT16(s, fieldsPresent);
	Stream_Seek_UINT16(s); /* reserved */
	error = rpc_error_to_string(errorCode);
	WLog_DBG(TAG, "serverVersion=%" PRId16 ", errorCode=%s, fieldsPresent=%s", serverVersion, error,
	         tunnel_response_fields_present_to_string(fieldsPresent));

	if (FAILED(errorCode))
	{
		WLog_ERR(TAG, "Tunnel creation error %s", error);
		freerdp_set_last_error_log(rdg->context, errorCode);
		return FALSE;
	}

	return rdg_send_tunnel_authorization(rdg);
}