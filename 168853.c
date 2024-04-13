static BOOL rdg_process_unknown_packet(rdpRdg* rdg, int type)
{
	WINPR_UNUSED(rdg);
	WINPR_UNUSED(type);
	WLog_WARN(TAG, "Unknown Control Packet received: %X", type);
	return TRUE;
}