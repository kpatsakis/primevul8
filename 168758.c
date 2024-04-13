static BOOL rdg_process_control_packet(rdpRdg* rdg, int type, size_t packetLength)
{
	wStream* s = NULL;
	size_t readCount = 0;
	int status;
	size_t payloadSize = packetLength - sizeof(RdgPacketHeader);

	if (packetLength < sizeof(RdgPacketHeader))
		return FALSE;

	assert(sizeof(RdgPacketHeader) < INT_MAX);

	if (payloadSize)
	{
		s = Stream_New(NULL, payloadSize);

		if (!s)
			return FALSE;

		while (readCount < payloadSize)
		{
			status =
			    BIO_read(rdg->tlsOut->bio, Stream_Pointer(s), (int)payloadSize - (int)readCount);

			if (status <= 0)
			{
				if (!BIO_should_retry(rdg->tlsOut->bio))
				{
					Stream_Free(s, TRUE);
					return FALSE;
				}

				continue;
			}

			Stream_Seek(s, (size_t)status);
			readCount += (size_t)status;

			if (readCount > INT_MAX)
			{
				Stream_Free(s, TRUE);
				return FALSE;
			}
		}
	}

	switch (type)
	{
		case PKT_TYPE_CLOSE_CHANNEL:
			EnterCriticalSection(&rdg->writeSection);
			status = rdg_process_close_packet(rdg);
			LeaveCriticalSection(&rdg->writeSection);
			break;

		case PKT_TYPE_KEEPALIVE:
			EnterCriticalSection(&rdg->writeSection);
			status = rdg_process_keep_alive_packet(rdg);
			LeaveCriticalSection(&rdg->writeSection);
			break;

		default:
			status = rdg_process_unknown_packet(rdg, type);
			break;
	}

	Stream_Free(s, TRUE);
	return status;
}