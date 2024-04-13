void G711::runPush()
{
	AFframecount framesToWrite = m_inChunk->frameCount;
	AFframecount samplesToWrite = m_inChunk->frameCount * m_inChunk->f.channelCount;
	int framesize = m_inChunk->f.channelCount;

	assert(m_track->f.compressionType == AF_COMPRESSION_G711_ULAW ||
		m_track->f.compressionType == AF_COMPRESSION_G711_ALAW);

	/* Compress frames into i->outc. */

	if (m_track->f.compressionType == AF_COMPRESSION_G711_ULAW)
		linear2ulaw_buf(static_cast<const int16_t *>(m_inChunk->buffer),
			static_cast<uint8_t *>(m_outChunk->buffer), samplesToWrite);
	else
		linear2alaw_buf(static_cast<const int16_t *>(m_inChunk->buffer),
			static_cast<uint8_t *>(m_outChunk->buffer), samplesToWrite);

	/* Write the compressed data. */

	ssize_t bytesWritten = write(m_outChunk->buffer, framesize * framesToWrite);
	AFframecount framesWritten = bytesWritten >= 0 ? bytesWritten / framesize : 0;

	CHNK(printf("writing %d frames to g711 file\n", framesToWrite));

	if (framesWritten != framesToWrite)
	{
		/* report error if we haven't already */
		if (m_track->filemodhappy)
		{
			/* i/o error */
			if (framesWritten < 0)
				_af_error(AF_BAD_WRITE,
					"unable to write data (%s) -- "
					"wrote %d out of %d frames",
					strerror(errno),
					m_track->nextfframe + framesWritten,
					m_track->nextfframe + framesToWrite);

			/* usual disk full error */
			else
				_af_error(AF_BAD_WRITE,
					"unable to write data (disk full) -- "
					"wrote %d out of %d frames",
					m_track->nextfframe + framesWritten,
					m_track->nextfframe + framesToWrite);

			m_track->filemodhappy = false;
		}
	}

	m_track->nextfframe += framesWritten;
	m_track->totalfframes = m_track->nextfframe;
	m_track->fpos_next_frame += (framesWritten>0) ? framesWritten*framesize : 0;

	assert(!canSeek() || (tell() == m_track->fpos_next_frame));
}