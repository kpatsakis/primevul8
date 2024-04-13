status ModuleState::sync(AFfilehandle file, _Track *track)
{
	track->filemodhappy = true;
	for (std::vector<SharedPtr<Module> >::reverse_iterator i=m_modules.rbegin();
			i != m_modules.rend(); ++i)
		(*i)->sync1();
	if (!track->filemodhappy)
		return AF_FAIL;
	for (std::vector<SharedPtr<Module> >::iterator i=m_modules.begin();
			i != m_modules.end(); ++i)
		(*i)->sync2();
	return AF_SUCCEED;
}