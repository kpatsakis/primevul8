AFfilehandle afOpenFD (int fd, const char *mode, AFfilesetup setup)
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

	vf = new File(fd, access == _AF_READ_ACCESS ?
		File::ReadAccess : File::WriteAccess);

	if (_afOpenFile(access, vf, NULL, &filehandle, setup) != AF_SUCCEED)
		af_fclose(vf);

	return filehandle;
}