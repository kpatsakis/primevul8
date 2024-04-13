int afGetFileFormat (AFfilehandle file, int *version)
{
	if (!_af_filehandle_ok(file))
		return -1;

	if (version != NULL)
	{
		if (_af_units[file->fileFormat].getversion)
			*version = _af_units[file->fileFormat].getversion(file);
		else
			*version = 0;
	}

	return file->fileFormat;
}