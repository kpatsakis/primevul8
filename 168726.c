static BOOL rdg_read_all(rdpTls* tls, BYTE* buffer, int size)
{
	int status;
	int readCount = 0;
	BYTE* pBuffer = buffer;

	while (readCount < size)
	{
		status = BIO_read(tls->bio, pBuffer, size - readCount);

		if (status <= 0)
		{
			if (!BIO_should_retry(tls->bio))
				return FALSE;

			continue;
		}

		readCount += status;
		pBuffer += status;
	}

	return TRUE;
}