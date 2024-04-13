void afInitFileFormat (AFfilesetup setup, int filefmt)
{
	if (!_af_filesetup_ok(setup))
		return;

	if (filefmt < 0 || filefmt > _AF_NUM_UNITS)
	{
		_af_error(AF_BAD_FILEFMT, "unrecognized file format %d",
			filefmt);
		return;
	}

	if (!_af_units[filefmt].implemented)
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED,
			"%s format not currently supported",
			_af_units[filefmt].name);
		return;
	}

	setup->fileFormat = filefmt;
}