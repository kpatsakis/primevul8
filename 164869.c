	virtual void run(Chunk &inChunk, Chunk &outChunk)
	{
		const void *src = inChunk.buffer;
		void *dst = outChunk.buffer;
		int count = inChunk.frameCount * inChunk.f.channelCount;

		switch (m_format)
		{
			case kInt8:
				run<int8_t>(src, dst, count); break;
			case kInt16:
				run<int16_t>(src, dst, count); break;
			case kInt24:
			case kInt32:
				run<int32_t>(src, dst, count); break;
			case kFloat:
				run<float>(src, dst, count); break;
			case kDouble:
				run<double>(src, dst, count); break;
			default:
				assert(false);
		}
	}