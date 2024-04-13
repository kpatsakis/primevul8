static BOOL printer_config_valid(const char* path)
{
	if (!path)
		return FALSE;

	if (!PathFileExistsA(path))
		return FALSE;

	return TRUE;
}