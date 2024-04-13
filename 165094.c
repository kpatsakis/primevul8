void G711::runPull()
{
	AFframecount framesToRead = m_outChunk->frameCount;
	AFframecount samplesToRead = m_outChunk->frameCount * m_outChunk->f.channelCount;
	int framesize = m_outChunk->f.channelCount;

	/* Read the compressed frames. */

	ssize_t bytesRead = read(m_inChunk->buffer, framesize * framesToRead);
	AFframecount framesRead = bytesRead >= 0 ? bytesRead / framesize : 0;

	/* Decompress into i->outc. */

	if (m_track->f.compressionType == AF_COMPRESSION_G711_ULAW)
		ulaw2linear_buf(static_cast<const uint8_t *>(m_inChunk->buffer),
			static_cast<int16_t *>(m_outChunk->buffer), samplesToRead);
	else
		alaw2linear_buf(static_cast<const uint8_t *>(m_inChunk->buffer),
			static_cast<int16_t *>(m_outChunk->buffer), samplesToRead);

	CHNK(printf("reading %d frames from g711 file (got %d)\n",
		framesToRead, framesRead));

	m_track->nextfframe += framesRead;
	m_track->fpos_next_frame += (framesRead>0) ? framesRead*framesize : 0;
	assert(!canSeek() || (tell() == m_track->fpos_next_frame));

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