AFfilehandle afOpenFile (const char *filename, const char *mode, AFfilesetup setup)
{
	FILE		*fp;
	AFvirtualfile	*vf;
	AFfilehandle	filehandle;
	int		access;

	if (mode == NULL)
	{
		_af_error(AF_BAD_ACCMODE, "null access mode");
		return AF_NULL_FILEHANDLE;
	}

	if (mode[0] == 'r')
		access = _AF_READ_ACCESS;
	else if (mode[0] == 'w')
		access = _AF_WRITE_ACCESS;
	else
	{
		_af_error(AF_BAD_ACCMODE, "unrecognized access mode '%s'", mode);
		return AF_NULL_FILEHANDLE;
	}

	vf = File::open(filename,
		access == _AF_READ_ACCESS ? File::ReadAccess : File::WriteAccess);
	if (vf == NULL)
	{
		_af_error(AF_BAD_OPEN, "could not open file '%s'", filename);
		return AF_NULL_FILEHANDLE;
	}

	if (_afOpenFile(access, vf, filename, &filehandle, setup) != AF_SUCCEED)
		af_fclose(vf);

	return filehandle;
}