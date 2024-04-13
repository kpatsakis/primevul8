	void run(const void *input, void *output, int count)
	{
		const T *in = reinterpret_cast<const T *>(input);
		uint8_t *out = reinterpret_cast<uint8_t *>(output);
		for (int i=0; i<count; i++)
		{
			uint8_t c0, c1, c2;
			extract3(in[i], c0, c1, c2);
			out[3*i] = c0;
			out[3*i+1] = c1;
			out[3*i+2] = c2;
		}
	}