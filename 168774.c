static BOOL rdg_process_channel_response(rdpRdg* rdg, wStream* s)
{
	UINT16 fieldsPresent;
	UINT32 errorCode;
	const char* error;
	WLog_DBG(TAG, "Channel response received");

	if (rdg->state != RDG_CLIENT_STATE_CHANNEL_CREATE)
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
	WLog_DBG(TAG, "channel response errorCode=%s, fieldsPresent=%s", error,
	         channel_response_fields_present_to_string(fieldsPresent));

	if (FAILED(errorCode))
	{
		WLog_ERR(TAG, "channel response errorCode=%s, fieldsPresent=%s", error,
		         channel_response_fields_present_to_string(fieldsPresent));
		freerdp_set_last_error_log(rdg->context, errorCode);
		return FALSE;
	}

	rdg->state = RDG_CLIENT_STATE_OPENED;
	return TRUE;
}