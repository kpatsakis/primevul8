	virtual void describe()
	{
		m_outChunk->f.sampleFormat = m_outFormat == kDouble ?
			AF_SAMPFMT_DOUBLE : AF_SAMPFMT_FLOAT;
		m_outChunk->f.sampleWidth = m_outFormat == kDouble ? 64 : 32;
	}