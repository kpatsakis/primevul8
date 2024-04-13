	virtual void run(Chunk &inChunk, Chunk &outChunk)
	{
		switch (m_inChunk->f.bytesPerSample(false))
		{
			case 2:
				run<2, int16_t>(inChunk, outChunk); break;
			case 3:
				run<3, char>(inChunk, outChunk); break;
			case 4:
				run<4, int32_t>(inChunk, outChunk); break;
			case 8:
				run<8, int64_t>(inChunk, outChunk); break;
			default:
				assert(false); break;
		}
	}