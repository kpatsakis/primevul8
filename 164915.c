	virtual void run(Chunk &inChunk, Chunk &outChunk)
	{
		const void *src = inChunk.buffer;
		void *dst = outChunk.buffer;
		size_t count = inChunk.frameCount * inChunk.f.channelCount;

		switch (m_outFormat)
		{
			case kFloat:
				transform<floatToFloat<double, float> >(src, dst, count);
				break;
			case kDouble:
				transform<floatToFloat<float, double> >(src, dst, count);
				break;
			default:
				assert(false);
		}
	}