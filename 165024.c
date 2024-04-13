void IMA::runPull()
{
	AFframecount framesToRead = m_outChunk->frameCount;
	AFframecount framesRead = 0;

	assert(m_outChunk->frameCount % m_framesPerBlock == 0);
	int blockCount = m_outChunk->frameCount / m_framesPerBlock;

	/* Read the compressed frames. */
	ssize_t bytesRead = read(m_inChunk->buffer, m_blockAlign * blockCount);
	ssize_t blocksRead = bytesRead >= 0 ? bytesRead / m_blockAlign : 0;

	/* This condition would indicate that the file is bad. */
	if (blocksRead < 0)
	{
		if (m_track->filemodhappy)
		{
			_af_error(AF_BAD_READ, "file missing data");
			m_track->filemodhappy = false;
		}
	}

	if (blocksRead < blockCount)
		blockCount = blocksRead;

	/* Decompress into module->outc. */
	for (int i=0; i<blockCount; i++)
	{
		ssize_t bytesDecoded = decodeBlock(static_cast<const uint8_t *>(m_inChunk->buffer) + i * m_blockAlign,
			static_cast<int16_t *>(m_outChunk->buffer) + i * m_framesPerBlock * m_track->f.channelCount);

		framesRead += m_framesPerBlock;
	}

	m_track->nextfframe += framesRead;

	if (blocksRead > 0)
		m_track->fpos_next_frame += blocksRead * m_blockAlign;

	assert(tell() == m_track->fpos_next_frame);

	/*
		If we got EOF from read, then we return the actual amount read.

		Complain only if there should have been more frames in the file.
	*/

	if (m_track->totalfframes != -1 && framesRead != framesToRead)
	{
		/* Report error if we haven't already */
		if (m_track->filemodhappy)
		{
			_af_error(AF_BAD_READ,
				"file missing data -- read %d frames, should be %d",
				m_track->nextfframe,
				m_track->totalfframes);
			m_track->filemodhappy = false;
		}
	}

	m_outChunk->frameCount = framesRead;
}