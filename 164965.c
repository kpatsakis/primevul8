int afIdentifyNamedFD (int fd, const char *filename, int *implemented)
{
	FILE		*fp;
	AFvirtualfile	*vf;
	int		result;

	/*
		Duplicate the file descriptor since otherwise the
		original file descriptor would get closed when we close
		the virtual file below.
	*/
	fd = dup(fd);

	vf = new File(fd, File::ReadAccess);
	if (vf == NULL)
	{
		_af_error(AF_BAD_OPEN, "could not open file '%s'", filename);
		return AF_FILE_UNKNOWN;
	}

	result = _af_identify(vf, implemented);

	af_fclose(vf);

	return result;
}