int afCloseFile (AFfilehandle file)
{
	int	err;

	if (!_af_filehandle_ok(file))
		return -1;

	afSyncFile(file);

	err = af_fclose(file->fh);
	if (err < 0)
		_af_error(AF_BAD_CLOSE, "close returned %d", err);

	freeFileHandle(file);

	return 0;
}