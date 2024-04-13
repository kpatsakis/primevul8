	void run(const void *srcData, void *dstData, int count)
	{
		const T minValue = m_outputMapping.minClip;
		const T maxValue = m_outputMapping.maxClip;

		const T *src = reinterpret_cast<const T *>(srcData);
		T *dst = reinterpret_cast<T *>(dstData);

		for (int i=0; i<count; i++)
		{
			T t = src[i];
			t = std::min(t, maxValue);
			t = std::max(t, minValue);
			dst[i] = t;
		}
	}