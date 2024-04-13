static int rdg_write_data_packet(rdpRdg* rdg, const BYTE* buf, int isize)
{
	int status;
	size_t s;
	wStream* sChunk;
	size_t size = (size_t)isize;
	size_t packetSize = size + 10;
	char chunkSize[11];

	if ((isize < 0) || (isize > UINT16_MAX))
		return -1;

	if (size < 1)
		return 0;

	sprintf_s(chunkSize, sizeof(chunkSize), "%" PRIxz "\r\n", packetSize);
	sChunk = Stream_New(NULL, strnlen(chunkSize, sizeof(chunkSize)) + packetSize + 2);

	if (!sChunk)
		return -1;

	Stream_Write(sChunk, chunkSize, strnlen(chunkSize, sizeof(chunkSize)));
	Stream_Write_UINT16(sChunk, PKT_TYPE_DATA);      /* Type */
	Stream_Write_UINT16(sChunk, 0);                  /* Reserved */
	Stream_Write_UINT32(sChunk, (UINT32)packetSize); /* Packet length */
	Stream_Write_UINT16(sChunk, (UINT16)size);       /* Data size */
	Stream_Write(sChunk, buf, size);                 /* Data */
	Stream_Write(sChunk, "\r\n", 2);
	Stream_SealLength(sChunk);
	s = Stream_Length(sChunk);

	if (s > INT_MAX)
		return -1;

	status = tls_write_all(rdg->tlsIn, Stream_Buffer(sChunk), (int)s);
	Stream_Free(sChunk, TRUE);

	if (status < 0)
		return -1;

	return (int)size;
}