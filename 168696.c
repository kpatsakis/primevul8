static UINT rdpei_on_data_received(IWTSVirtualChannelCallback* pChannelCallback, wStream* data)
{
	RDPEI_CHANNEL_CALLBACK* callback = (RDPEI_CHANNEL_CALLBACK*)pChannelCallback;
	return rdpei_recv_pdu(callback, data);
}