static BOOL rdg_write_packet(rdpRdg* rdg, wStream* sPacket)
{
	size_t s;
	int status;
	wStream* sChunk;
	char chunkSize[11];
	sprintf_s(chunkSize, sizeof(chunkSize), "%" PRIXz "\r\n", Stream_Length(sPacket));
	sChunk = Stream_New(NULL, strnlen(chunkSize, sizeof(chunkSize)) + Stream_Length(sPacket) + 2);

	if (!sChunk)
		return FALSE;

	Stream_Write(sChunk, chunkSize, strnlen(chunkSize, sizeof(chunkSize)));
	Stream_Write(sChunk, Stream_Buffer(sPacket), Stream_Length(sPacket));
	Stream_Write(sChunk, "\r\n", 2);
	Stream_SealLength(sChunk);
	s = Stream_Length(sChunk);

	if (s > INT_MAX)
		return FALSE;

	status = tls_write_all(rdg->tlsIn, Stream_Buffer(sChunk), (int)s);
	Stream_Free(sChunk, TRUE);

	if (status < 0)
		return FALSE;

	return TRUE;
}