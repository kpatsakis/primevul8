static UINT rdpei_plugin_terminated(IWTSPlugin* pPlugin)
{
	RDPEI_PLUGIN* rdpei = (RDPEI_PLUGIN*)pPlugin;

	if (!pPlugin)
		return ERROR_INVALID_PARAMETER;

	free(rdpei->listener_callback);
	free(rdpei->context);
	free(rdpei);
	return CHANNEL_RC_OK;
}