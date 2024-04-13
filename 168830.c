static BOOL rdg_process_tunnel_authorization_response(rdpRdg* rdg, wStream* s)
{
	UINT32 errorCode;
	UINT16 fieldsPresent;
	const char* error;
	WLog_DBG(TAG, "Tunnel authorization received");

	if (rdg->state != RDG_CLIENT_STATE_TUNNEL_AUTHORIZE)
	{
		return FALSE;
	}

	if (Stream_GetRemainingLength(s) < 8)
	{
		WLog_ERR(TAG, "[%s] Short packet %" PRIuz ", expected 8", __FUNCTION__,
		         Stream_GetRemainingLength(s));
		return FALSE;
	}

	Stream_Read_UINT32(s, errorCode);
	Stream_Read_UINT16(s, fieldsPresent);
	Stream_Seek_UINT16(s); /* reserved */
	error = rpc_error_to_string(errorCode);
	WLog_DBG(TAG, "errorCode=%s, fieldsPresent=%s", error,
	         tunnel_authorization_response_fields_present_to_string(fieldsPresent));

	if (FAILED(errorCode))
	{
		WLog_ERR(TAG, "Tunnel authorization error %s", error);
		freerdp_set_last_error_log(rdg->context, errorCode);
		return FALSE;
	}

	return rdg_send_channel_create(rdg);
}