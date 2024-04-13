	ConvertFloat(FormatCode inFormat, FormatCode outFormat) :
		m_inFormat(inFormat), m_outFormat(outFormat)
	{
		assert((m_inFormat == kFloat && m_outFormat == kDouble) ||
			(m_inFormat == kDouble && m_outFormat == kFloat));
	}