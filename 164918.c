	virtual void describe()
	{
		switch (m_outFormat)
		{
			case kFloat:
				m_outChunk->f.sampleFormat = AF_SAMPFMT_FLOAT;
				m_outChunk->f.sampleWidth = 32;
				break;
			case kDouble:
				m_outChunk->f.sampleFormat = AF_SAMPFMT_DOUBLE;
				m_outChunk->f.sampleWidth = 64;
				break;
			default:
				assert(false);
		}
	}