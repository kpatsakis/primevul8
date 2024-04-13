	void run(Chunk &inChunk, Chunk &outChunk)
	{
		int sampleCount = inChunk.f.channelCount * inChunk.frameCount;
		runSwap<N, T>(reinterpret_cast<const T *>(inChunk.buffer),
			reinterpret_cast<T *>(outChunk.buffer),
			sampleCount);
	}