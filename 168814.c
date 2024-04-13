static BOOL rdg_process_close_packet(rdpRdg* rdg)
{
	int status = -1;
	size_t s;
	wStream* sChunk;
	UINT32 packetSize = 12;
	char chunkSize[11];
	int chunkLen = sprintf_s(chunkSize, sizeof(chunkSize), "%" PRIx32 "\r\n", packetSize);

	if (chunkLen < 0)
		return FALSE;

	sChunk = Stream_New(NULL, (size_t)chunkLen + packetSize + 2);

	if (!sChunk)
		return FALSE;

	Stream_Write(sChunk, chunkSize, (size_t)chunkLen);
	Stream_Write_UINT16(sChunk, PKT_TYPE_CLOSE_CHANNEL_RESPONSE); /* Type */
	Stream_Write_UINT16(sChunk, 0);                               /* Reserved */
	Stream_Write_UINT32(sChunk, packetSize);                      /* Packet length */
	Stream_Write_UINT32(sChunk, 0);                               /* Status code */
	Stream_Write(sChunk, "\r\n", 2);
	Stream_SealLength(sChunk);
	s = Stream_Length(sChunk);

	if (s <= INT_MAX)
		status = tls_write_all(rdg->tlsIn, Stream_Buffer(sChunk), (int)s);

	Stream_Free(sChunk, TRUE);
	return (status < 0 ? FALSE : TRUE);
}