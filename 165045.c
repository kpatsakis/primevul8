	virtual void run(Chunk &inChunk, Chunk &outChunk)
	{
		int count = inChunk.frameCount * inChunk.f.channelCount;
		if (m_format == kFloat)
			run<float>(inChunk.buffer, outChunk.buffer, count);
		else if (m_format == kDouble)
			run<double>(inChunk.buffer, outChunk.buffer, count);
		else
			assert(false);
	}