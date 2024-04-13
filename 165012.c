	Transform(FormatCode format,
		const _PCMInfo &inputMapping,
		const _PCMInfo &outputMapping) :
		m_format(format),
		m_inputMapping(inputMapping),
		m_outputMapping(outputMapping)
	{
		assert(m_format == kFloat || m_format == kDouble);
	}