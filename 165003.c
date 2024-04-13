	virtual void run(Chunk &inChunk, Chunk &outChunk)
	{
		const void *src = inChunk.buffer;
		void *dst = outChunk.buffer;
		int count = inChunk.frameCount * inChunk.f.channelCount;

		if (m_inputFormat == kFloat)
		{
			switch (m_outputFormat)
			{
				case kInt8:
					run<float, int8_t>(src, dst, count); break;
				case kInt16:
					run<float, int16_t>(src, dst, count); break;
				case kInt24:
				case kInt32:
					run<float, int32_t>(src, dst, count); break;
				default:
					assert(false);
			}
		}
		else if (m_inputFormat == kDouble)
		{
			switch (m_outputFormat)
			{
				case kInt8:
					run<double, int8_t>(src, dst, count); break;
				case kInt16:
					run<double, int16_t>(src, dst, count); break;
				case kInt24:
				case kInt32:
					run<double, int32_t>(src, dst, count); break;
				default:
					assert(false);
			}
		}
	}