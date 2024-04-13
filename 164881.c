void G711::describe()
{
	if (mode() == Compress)
	{
		m_outChunk->f.compressionType = m_track->f.compressionType;
	}
	else
	{
		m_outChunk->f.byteOrder = _AF_BYTEORDER_NATIVE;
		m_outChunk->f.compressionType = AF_COMPRESSION_NONE;
	}
}