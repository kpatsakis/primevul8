	ConvertInt(FormatCode inFormat, FormatCode outFormat) :
		m_inFormat(inFormat),
		m_outFormat(outFormat)
	{
		assert(isInteger(m_inFormat));
		assert(isInteger(m_outFormat));
	}