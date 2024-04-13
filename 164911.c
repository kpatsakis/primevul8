status ModuleState::reset(AFfilehandle file, _Track *track)
{
	track->filemodhappy = true;
	for (std::vector<SharedPtr<Module> >::reverse_iterator i=m_modules.rbegin();
			i != m_modules.rend(); ++i)
		(*i)->reset1();
	track->frames2ignore = 0;
	if (!track->filemodhappy)
		return AF_FAIL;
	for (std::vector<SharedPtr<Module> >::iterator i=m_modules.begin();
			i != m_modules.end(); ++i)
		(*i)->reset2();
	if (!track->filemodhappy)
		return AF_FAIL;
	return AF_SUCCEED;
}