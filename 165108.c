bool _af_filesetup_ok (AFfilesetup setup)
{
	if (setup == AF_NULL_FILESETUP)
	{
		_af_error(AF_BAD_FILESETUP, "null file setup");
		return false;
	}
	if (setup->valid != _AF_VALID_FILESETUP)
	{
		_af_error(AF_BAD_FILESETUP, "invalid file setup");
		return false;
	}
	return true;
}