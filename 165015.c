bool _af_filehandle_ok (AFfilehandle file)
{
	if (file == AF_NULL_FILEHANDLE)
	{
		_af_error(AF_BAD_FILEHANDLE, "null file handle");
		return false;
	}
	if (file->valid != _AF_VALID_FILEHANDLE)
	{
		_af_error(AF_BAD_FILEHANDLE, "invalid file handle");
		return false;
	}
	return true;
}