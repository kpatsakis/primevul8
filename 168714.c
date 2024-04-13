static UINT rdpei_on_new_channel_connection(IWTSListenerCallback* pListenerCallback,
                                            IWTSVirtualChannel* pChannel, BYTE* Data,
                                            BOOL* pbAccept, IWTSVirtualChannelCallback** ppCallback)
{
	RDPEI_CHANNEL_CALLBACK* callback;
	RDPEI_LISTENER_CALLBACK* listener_callback = (RDPEI_LISTENER_CALLBACK*)pListenerCallback;
	callback = (RDPEI_CHANNEL_CALLBACK*)calloc(1, sizeof(RDPEI_CHANNEL_CALLBACK));

	if (!callback)
	{
		WLog_ERR(TAG, "calloc failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	callback->iface.OnDataReceived = rdpei_on_data_received;
	callback->iface.OnClose = rdpei_on_close;
	callback->plugin = listener_callback->plugin;
	callback->channel_mgr = listener_callback->channel_mgr;
	callback->channel = pChannel;
	listener_callback->channel_callback = callback;
	*ppCallback = (IWTSVirtualChannelCallback*)callback;
	return CHANNEL_RC_OK;
}