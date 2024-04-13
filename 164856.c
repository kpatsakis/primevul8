bool _af_filehandle_can_read (AFfilehandle file)
{
	if (file->access != _AF_READ_ACCESS)
	{
		_af_error(AF_BAD_NOREADACC, "file not opened for read access");
		return false;
	}

	return true;
}