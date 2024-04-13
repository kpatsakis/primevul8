void ApplyChannelMatrix::describe()
{
	m_outChunk->f.channelCount = m_outChannels;
	m_outChunk->f.pcm.minClip = m_minClip;
	m_outChunk->f.pcm.maxClip = m_maxClip;
}