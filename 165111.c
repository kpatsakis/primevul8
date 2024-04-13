	virtual void run(Chunk &inChunk, Chunk &outChunk)
	{
		const void *src = inChunk.buffer;
		void *dst = outChunk.buffer;
		size_t count = inChunk.frameCount * inChunk.f.channelCount;

#define MASK(N, M) (((N)<<3) | (M))
#define HANDLE(N, M) \
	case MASK(N, M): convertInt<N, M>(src, dst, count); break;
		switch (MASK(m_inFormat, m_outFormat))
		{
			HANDLE(kInt8, kInt16)
			HANDLE(kInt8, kInt24)
			HANDLE(kInt8, kInt32)
			HANDLE(kInt16, kInt8)
			HANDLE(kInt16, kInt24)
			HANDLE(kInt16, kInt32)
			HANDLE(kInt24, kInt8)
			HANDLE(kInt24, kInt16)
			HANDLE(kInt24, kInt32)
			HANDLE(kInt32, kInt8)
			HANDLE(kInt32, kInt16)
			HANDLE(kInt32, kInt24)
		}
#undef MASK
#undef HANDLE
	}