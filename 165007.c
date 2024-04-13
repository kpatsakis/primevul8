int afIdentifyFD (int fd)
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

	result = _af_identify(vf, NULL);

	af_fclose(vf);

	return result;
}