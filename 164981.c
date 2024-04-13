	virtual void run(Chunk &input, Chunk &output)
	{
		size_t count = input.frameCount * m_inChunk->f.channelCount;
		if (m_fromSigned)
			convertSignedToUnsigned(input.buffer, output.buffer, count);
		else
			convertUnsignedToSigned(input.buffer, output.buffer, count);
	}