	virtual void run(Chunk &inChunk, Chunk &outChunk)
	{
		int count = inChunk.f.channelCount * inChunk.frameCount;
		if (m_isSigned)
			run<int32_t>(reinterpret_cast<const uint8_t *>(inChunk.buffer),
				reinterpret_cast<int32_t *>(outChunk.buffer),
				count);
		else
			run<uint32_t>(reinterpret_cast<const uint8_t *>(inChunk.buffer),
				reinterpret_cast<uint32_t *>(outChunk.buffer),
				count);
	}