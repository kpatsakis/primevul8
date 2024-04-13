	void run(const uint8_t *input, T *output, int sampleCount)
	{
		for (int i=0; i<sampleCount; i++)
		{
			T t =
#ifdef WORDS_BIGENDIAN
				(input[3*i] << 24) |
				(input[3*i+1] << 16) |
				input[3*i+2] << 8;
#else
				(input[3*i+2] << 24) |
				(input[3*i+1] << 16) |
				input[3*i] << 8;
#endif
			output[i] = t >> 8;
		}
	}