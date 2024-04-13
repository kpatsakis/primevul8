bool _af_filehandle_can_write (AFfilehandle file)
{
	if (file->access != _AF_WRITE_ACCESS)
	{
		_af_error(AF_BAD_NOWRITEACC, "file not opened for write access");
		return false;
	}

	return true;
}