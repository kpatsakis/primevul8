	virtual void describe()
	{
		m_outChunk->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
		m_outChunk->f.sampleWidth = (m_outputFormat + 1) * CHAR_BIT;
		m_outChunk->f.pcm = m_outputMapping;
	}