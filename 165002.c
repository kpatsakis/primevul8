AFfilehandle afOpenVirtualFile (AFvirtualfile *vfile, const char *mode,
	AFfilesetup setup)
{
	AFfilehandle	filehandle;
	int		access; 

	if (vfile == NULL)
	{
		_af_error(AF_BAD_FILEHANDLE, "null virtual filehandle");
		return AF_NULL_FILEHANDLE;
	}

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

	if (_afOpenFile(access, vfile, NULL, &filehandle, setup) != AF_SUCCEED)
		af_fclose(vfile);

	return filehandle;
}