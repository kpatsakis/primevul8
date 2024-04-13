static UINT rdpei_recv_sc_ready_pdu(RDPEI_CHANNEL_CALLBACK* callback, wStream* s)
{
	UINT32 protocolVersion;
	Stream_Read_UINT32(s, protocolVersion); /* protocolVersion (4 bytes) */
#if 0

	if (protocolVersion != RDPINPUT_PROTOCOL_V10)
	{
		WLog_ERR(TAG,  "Unknown [MS-RDPEI] protocolVersion: 0x%08"PRIX32"", protocolVersion);
		return -1;
	}

#endif
	return CHANNEL_RC_OK;
}