static BOOL rdg_send_handshake(rdpRdg* rdg)
{
	wStream* s;
	BOOL status;
	s = Stream_New(NULL, 14);

	if (!s)
		return FALSE;

	Stream_Write_UINT16(s, PKT_TYPE_HANDSHAKE_REQUEST); /* Type (2 bytes) */
	Stream_Write_UINT16(s, 0);                          /* Reserved (2 bytes) */
	Stream_Write_UINT32(s, 14);                         /* PacketLength (4 bytes) */
	Stream_Write_UINT8(s, 1);                           /* VersionMajor (1 byte) */
	Stream_Write_UINT8(s, 0);                           /* VersionMinor (1 byte) */
	Stream_Write_UINT16(s, 0);                          /* ClientVersion (2 bytes), must be 0 */
	Stream_Write_UINT16(s, rdg->extAuth);               /* ExtendedAuthentication (2 bytes) */
	Stream_SealLength(s);
	status = rdg_write_packet(rdg, s);
	Stream_Free(s, TRUE);

	if (status)
	{
		rdg->state = RDG_CLIENT_STATE_HANDSHAKE;
	}

	return status;
}