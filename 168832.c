static UINT rdpei_recv_resume_touch_pdu(RDPEI_CHANNEL_CALLBACK* callback, wStream* s)
{
	RdpeiClientContext* rdpei = (RdpeiClientContext*)callback->plugin->pInterface;
	UINT error = CHANNEL_RC_OK;
	IFCALLRET(rdpei->ResumeTouch, error, rdpei);

	if (error)
		WLog_ERR(TAG, "rdpei->ResumeTouch failed with error %" PRIu32 "!", error);

	return error;
}