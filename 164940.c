	virtual void describe()
	{
		m_outChunk->f.byteOrder = m_inChunk->f.byteOrder == AF_BYTEORDER_BIGENDIAN ?
			AF_BYTEORDER_LITTLEENDIAN : AF_BYTEORDER_BIGENDIAN;
	}