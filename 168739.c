static UINT rdpei_recv_suspend_touch_pdu(RDPEI_CHANNEL_CALLBACK* callback, wStream* s)
{
	RdpeiClientContext* rdpei = (RdpeiClientContext*)callback->plugin->pInterface;
	UINT error = CHANNEL_RC_OK;
	IFCALLRET(rdpei->SuspendTouch, error, rdpei);

	if (error)
		WLog_ERR(TAG, "rdpei->SuspendTouch failed with error %" PRIu32 "!", error);

	return error;
}