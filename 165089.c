	void run(const void *srcData, void *dstData, int count)
	{
		const Input *src = reinterpret_cast<const Input *>(srcData);
		Output *dst = reinterpret_cast<Output *>(dstData);

		double m = m_outputMapping.slope / m_inputMapping.slope;
		double b = m_outputMapping.intercept - m * m_inputMapping.intercept;
		double minValue = m_outputMapping.minClip;
		double maxValue = m_outputMapping.maxClip;

		for (int i=0; i<count; i++)
		{
			double t = m * src[i] + b;
			t = std::min(t, maxValue);
			t = std::max(t, minValue);
			dst[i] = static_cast<Output>(t);
		}
	}