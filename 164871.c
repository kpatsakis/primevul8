void Module::push(size_t frames)
{
	m_outChunk->frameCount = frames;
	m_sink->runPush();
}