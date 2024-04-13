	virtual void describe()
	{
		getDefaultPCMMapping(m_outChunk->f.sampleWidth,
			m_outChunk->f.pcm.slope,
			m_outChunk->f.pcm.intercept,
			m_outChunk->f.pcm.minClip,
			m_outChunk->f.pcm.maxClip);
	}