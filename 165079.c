	void runSwap(const T *input, T *output, int sampleCount)
	{
		for (int i=0; i<sampleCount; i++)
			output[i] = byteswap(input[i]);
	}