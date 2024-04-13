status ModuleState::setup(AFfilehandle file, _Track *track)
{
	AFframecount fframepos = llrint(track->nextvframe * track->f.sampleRate / track->v.sampleRate);
	bool isReading = file->access == _AF_READ_ACCESS;

	if (!track->v.isUncompressed())
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED,
			"library does not support compression in virtual format yet");
		return AF_FAIL;
	}

	if (arrange(file, track) == AF_FAIL)
		return AF_FAIL;

	track->filemodhappy = true;
	int maxbufsize = 0;
	if (isReading)
	{
		m_chunks.back()->frameCount = _AF_ATOMIC_NVFRAMES;
		for (int i=m_modules.size() - 1; i >= 0; i--)
		{
			SharedPtr<Chunk> inChunk = m_chunks[i];
			SharedPtr<Chunk> outChunk = m_chunks[i+1];
			int bufsize = outChunk->frameCount * outChunk->f.bytesPerFrame(true);
			if (bufsize > maxbufsize)
				maxbufsize = bufsize;
			if (i != 0)
				m_modules[i]->setSource(m_modules[i-1].get());
			m_modules[i]->maxPull();
		}

		if (!track->filemodhappy)
			return AF_FAIL;
		int bufsize = m_fileModule->inChunk()->frameCount *
			m_fileModule->outChunk()->f.bytesPerFrame(true);
		if (bufsize > maxbufsize)
			maxbufsize = bufsize;
	}
	else
	{
		m_chunks.front()->frameCount = _AF_ATOMIC_NVFRAMES;
		for (size_t i=0; i<m_modules.size(); i++)
		{
			SharedPtr<Chunk> inChunk = m_chunks[i];
			SharedPtr<Chunk> outChunk = m_chunks[i+1];
			int bufsize = inChunk->frameCount * inChunk->f.bytesPerFrame(true);
			if (bufsize > maxbufsize)
				maxbufsize = bufsize;
			if (i != m_modules.size() - 1)
				m_modules[i]->setSink(m_modules[i+1].get());
			m_modules[i]->maxPush();
		}

		if (!track->filemodhappy)
			return AF_FAIL;

		int bufsize = m_fileModule->outChunk()->frameCount *
			m_fileModule->inChunk()->f.bytesPerFrame(true);
		if (bufsize > maxbufsize)
			maxbufsize = bufsize;
	}

	for (size_t i=0; i<m_chunks.size(); i++)
	{
		if ((isReading && i==m_chunks.size() - 1) || (!isReading && i==0))
			continue;
		m_chunks[i]->allocate(maxbufsize);
	}

	if (isReading)
	{
		if (track->totalfframes == -1)
			track->totalvframes = -1;
		else
			track->totalvframes = llrint(track->totalfframes *
				(track->v.sampleRate / track->f.sampleRate));

		track->nextfframe = fframepos;
		track->nextvframe = llrint(fframepos * track->v.sampleRate / track->f.sampleRate);

		m_isDirty = false;

		if (reset(file, track) == AF_FAIL)
			return AF_FAIL;
	}
	else
	{
		track->nextvframe = track->totalvframes =
			(AFframecount) (fframepos * track->v.sampleRate / track->f.sampleRate);
		m_isDirty = false;
	}

	return AF_SUCCEED;
}