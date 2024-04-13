static BOOL printer_remove_config(const rdpSettings* settings, const WCHAR* name, size_t length)
{
	BOOL rc = FALSE;
	char* path = get_printer_config_path(settings, name, length);

	if (!printer_config_valid(path))
		goto fail;

	rc = RemoveDirectoryA(path);
fail:
	free(path);
	return rc;
}