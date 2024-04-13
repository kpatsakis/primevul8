	virtual void run(Chunk &inChunk, Chunk &outChunk)
	{
		int count = inChunk.f.channelCount * inChunk.frameCount;
		if (m_isSigned)
			run<int32_t>(inChunk.buffer, outChunk.buffer, count);
		else
			run<uint32_t>(inChunk.buffer, outChunk.buffer, count);
	}