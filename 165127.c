	virtual void run(Chunk &inChunk, Chunk &outChunk)
	{
		const void *src = inChunk.buffer;
		void *dst = outChunk.buffer;
		int count = inChunk.frameCount * inChunk.f.channelCount;
		if (m_outFormat == kFloat)
		{
			switch (m_inFormat)
			{
				case kInt8:
					run<int8_t, float>(src, dst, count); break;
				case kInt16:
					run<int16_t, float>(src, dst, count); break;
				case kInt24:
				case kInt32:
					run<int32_t, float>(src, dst, count); break;
				default:
					assert(false);
			}
		}
		else if (m_outFormat == kDouble)
		{
			switch (m_inFormat)
			{
				case kInt8:
					run<int8_t, double>(src, dst, count); break;
				case kInt16:
					run<int16_t, double>(src, dst, count); break;
				case kInt24:
				case kInt32:
					run<int32_t, double>(src, dst, count); break;
				default:
					assert(false);
			}
		}
	}