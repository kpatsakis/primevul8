void ModuleState::addModule(Module *module)
{
	if (!module)
		return;

	m_modules.push_back(module);
	module->setInChunk(m_chunks.back().get());
	Chunk *chunk = new Chunk();
	chunk->f = m_chunks.back()->f;
	m_chunks.push_back(chunk);
	module->setOutChunk(chunk);
	module->describe();
}