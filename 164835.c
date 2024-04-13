status ModuleState::init(AFfilehandle file, _Track *track)
{
	if (initFileModule(file, track) == AF_FAIL)
		return AF_FAIL;

	return AF_SUCCEED;
}