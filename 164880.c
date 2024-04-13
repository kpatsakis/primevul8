void SimpleModule::runPush()
{
	m_outChunk->frameCount = m_inChunk->frameCount;
	run(*m_inChunk, *m_outChunk);
	push(m_outChunk->frameCount);
}