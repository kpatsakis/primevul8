static UINT rdpei_on_close(IWTSVirtualChannelCallback* pChannelCallback)
{
	RDPEI_CHANNEL_CALLBACK* callback = (RDPEI_CHANNEL_CALLBACK*)pChannelCallback;
	free(callback);
	return CHANNEL_RC_OK;
}