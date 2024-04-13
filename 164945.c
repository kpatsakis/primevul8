void Module::pull(size_t frames)
{
	m_inChunk->frameCount = frames;
	m_source->runPull();
}