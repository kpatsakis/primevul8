	virtual void describe()
	{
		const int scaleBits = m_inChunk->f.bytesPerSample(false) * CHAR_BIT;
		m_outChunk->f.sampleFormat =
			m_fromSigned ? AF_SAMPFMT_UNSIGNED : AF_SAMPFMT_TWOSCOMP;
		double shift = -(1 << (scaleBits - 1));
		if (m_fromSigned)
			shift = -shift;
		m_outChunk->f.pcm.intercept += shift;
		m_outChunk->f.pcm.minClip += shift;
		m_outChunk->f.pcm.maxClip += shift;
	}