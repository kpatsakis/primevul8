static int rdg_read_data_packet(rdpRdg* rdg, BYTE* buffer, int size)
{
	RdgPacketHeader header;
	size_t readCount = 0;
	int readSize;
	int status;

	if (!rdg->packetRemainingCount)
	{
		assert(sizeof(RdgPacketHeader) < INT_MAX);

		while (readCount < sizeof(RdgPacketHeader))
		{
			status = BIO_read(rdg->tlsOut->bio, (BYTE*)(&header) + readCount,
			                  (int)sizeof(RdgPacketHeader) - (int)readCount);

			if (status <= 0)
			{
				if (!BIO_should_retry(rdg->tlsOut->bio))
					return -1;

				if (!readCount)
					return 0;

				BIO_wait_read(rdg->tlsOut->bio, 50);
				continue;
			}

			readCount += (size_t)status;

			if (readCount > INT_MAX)
				return -1;
		}

		if (header.type != PKT_TYPE_DATA)
		{
			status = rdg_process_control_packet(rdg, header.type, header.packetLength);

			if (!status)
				return -1;

			return 0;
		}

		readCount = 0;

		while (readCount < 2)
		{
			status = BIO_read(rdg->tlsOut->bio, (BYTE*)(&rdg->packetRemainingCount) + readCount,
			                  2 - (int)readCount);

			if (status < 0)
			{
				if (!BIO_should_retry(rdg->tlsOut->bio))
					return -1;

				BIO_wait_read(rdg->tlsOut->bio, 50);
				continue;
			}

			readCount += (size_t)status;
		}
	}

	readSize = (rdg->packetRemainingCount < size ? rdg->packetRemainingCount : size);
	status = BIO_read(rdg->tlsOut->bio, buffer, readSize);

	if (status <= 0)
	{
		if (!BIO_should_retry(rdg->tlsOut->bio))
		{
			return -1;
		}

		return 0;
	}

	rdg->packetRemainingCount -= status;
	return status;
}