void PCM::runPull()
{
	AFframecount framesToRead = m_outChunk->frameCount;

	/*
		WARNING: Due to the optimization explained at the end of
		arrangemodules(), the pcm file module cannot depend on
		the presence of the intermediate working buffer which
		_AFsetupmodules usually allocates for file modules in
		their input or output chunk (for reading or writing,
		respectively).

		Fortunately, the pcm module has no need for such a buffer.
	*/

	/*
		Limit the number of frames to be read to the number of
		frames left in the track.
	*/
	if (m_track->totalfframes != -1 &&
		m_track->nextfframe + framesToRead > m_track->totalfframes)
	{
		framesToRead = m_track->totalfframes - m_track->nextfframe;
	}

	ssize_t bytesRead = read(m_outChunk->buffer, m_bytesPerFrame * framesToRead);
	AFframecount framesRead = bytesRead >= 0 ? bytesRead / m_bytesPerFrame : 0;

	CHNK(printf("reading %" AF_FRAMECOUNT_PRINT_FMT " frames from pcm file "
		"(got %" AF_FRAMECOUNT_PRINT_FMT ")\n",
		framesToRead, framesRead));

	m_track->nextfframe += framesRead;
	m_track->fpos_next_frame += (framesRead>0) ? framesRead * m_bytesPerFrame : 0;
	assert(!canSeek() || (tell() == m_track->fpos_next_frame));

	/*
		If we got EOF from read, then we return the actual amount read.

		Complain only if there should have been more frames in the file.
	*/

	if (framesRead != framesToRead && m_track->totalfframes != -1)
	{
		/* Report error if we haven't already. */
		if (m_track->filemodhappy)
		{
			_af_error(AF_BAD_READ,
				"file missing data -- read %d frames, "
				"should be %d",
				m_track->nextfframe,
				m_track->totalfframes);
			m_track->filemodhappy = false;
		}
	}

	m_outChunk->frameCount = framesRead;
}