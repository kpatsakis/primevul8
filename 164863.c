	void run(const void *srcData, void *dstData, int count)
	{
		const T *src = reinterpret_cast<const T *>(srcData);
		T *dst = reinterpret_cast<T *>(dstData);

		double m = m_outputMapping.slope / m_inputMapping.slope;
		double b = m_outputMapping.intercept - m * m_inputMapping.intercept;

		for (int i=0; i<count; i++)
			dst[i] = m * src[i] + b;
	}