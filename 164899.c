	ConvertFloatToIntClip(FormatCode inputFormat, FormatCode outputFormat,
		const _PCMInfo &inputMapping, const _PCMInfo &outputMapping) :
		m_inputFormat(inputFormat),
		m_outputFormat(outputFormat),
		m_inputMapping(inputMapping),
		m_outputMapping(outputMapping)
	{
		assert(m_inputFormat == kFloat || m_inputFormat == kDouble);
		assert(m_outputFormat == kInt8 ||
			m_outputFormat == kInt16 ||
			m_outputFormat == kInt24 ||
			m_outputFormat == kInt32);
	}