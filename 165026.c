void ApplyChannelMatrix::run(const void *inputData, void *outputData, int frameCount)
{
	const T *input = reinterpret_cast<const T *>(inputData);
	T *output = reinterpret_cast<T *>(outputData);
	for (int frame=0; frame<frameCount; frame++)
	{
		const T *inputSave = input;
		const double *m = m_matrix;
		for (int outChannel=0; outChannel < m_outChannels; outChannel++)
		{
			input = inputSave;
			double t = 0;
			for (int inChannel=0; inChannel < m_inChannels; inChannel++)
				t += *input++ * *m++;
			*output++ = t;
		}
	}
}