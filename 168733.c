static int rdpei_get_version(RdpeiClientContext* context)
{
	RDPEI_PLUGIN* rdpei = (RDPEI_PLUGIN*)context->handle;
	return rdpei->version;
}