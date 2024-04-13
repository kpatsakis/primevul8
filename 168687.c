static UINT rdpei_plugin_initialize(IWTSPlugin* pPlugin, IWTSVirtualChannelManager* pChannelMgr)
{
	UINT error;
	RDPEI_PLUGIN* rdpei = (RDPEI_PLUGIN*)pPlugin;
	rdpei->listener_callback = (RDPEI_LISTENER_CALLBACK*)calloc(1, sizeof(RDPEI_LISTENER_CALLBACK));

	if (!rdpei->listener_callback)
	{
		WLog_ERR(TAG, "calloc failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	rdpei->listener_callback->iface.OnNewChannelConnection = rdpei_on_new_channel_connection;
	rdpei->listener_callback->plugin = pPlugin;
	rdpei->listener_callback->channel_mgr = pChannelMgr;

	if ((error = pChannelMgr->CreateListener(pChannelMgr, RDPEI_DVC_CHANNEL_NAME, 0,
	                                         (IWTSListenerCallback*)rdpei->listener_callback,
	                                         &(rdpei->listener))))
	{
		WLog_ERR(TAG, "ChannelMgr->CreateListener failed with error %" PRIu32 "!", error);
		goto error_out;
	}

	rdpei->listener->pInterface = rdpei->iface.pInterface;

	return error;
error_out:
	free(rdpei->listener_callback);
	return error;
}