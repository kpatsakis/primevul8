UINT rdpei_send_frame(RdpeiClientContext* context)
{
	UINT64 currentTime;
	RDPEI_PLUGIN* rdpei = (RDPEI_PLUGIN*)context->handle;
	RDPEI_CHANNEL_CALLBACK* callback = rdpei->listener_callback->channel_callback;
	UINT error;
	currentTime = GetTickCount64();

	if (!rdpei->previousFrameTime && !rdpei->currentFrameTime)
	{
		rdpei->currentFrameTime = currentTime;
		rdpei->frame.frameOffset = 0;
	}
	else
	{
		rdpei->currentFrameTime = currentTime;
		rdpei->frame.frameOffset = rdpei->currentFrameTime - rdpei->previousFrameTime;
	}

	if ((error = rdpei_send_touch_event_pdu(callback, &rdpei->frame)))
	{
		WLog_ERR(TAG, "rdpei_send_touch_event_pdu failed with error %" PRIu32 "!", error);
		return error;
	}

	rdpei->previousFrameTime = rdpei->currentFrameTime;
	rdpei->frame.contactCount = 0;
	return error;
}