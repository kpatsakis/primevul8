void ModuleState::print()
{
	fprintf(stderr, "modules:\n");
	for (size_t i=0; i<m_modules.size(); i++)
		fprintf(stderr, " %s (%p) in %p out %p\n",
			m_modules[i]->name(), m_modules[i].get(),
			m_modules[i]->inChunk(),
			m_modules[i]->outChunk());
	fprintf(stderr, "chunks:\n");
	for (size_t i=0; i<m_chunks.size(); i++)
		fprintf(stderr, " %p %s\n",
			m_chunks[i].get(),
			m_chunks[i]->f.description().c_str());
}